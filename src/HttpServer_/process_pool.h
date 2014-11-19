#ifndef _PROCESS_POOL_H
#define _PROCESS_POOL_H

#include <functional>

#include <queue>
#include <vector>

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

template <typename T>
class process_pool{
public:
	typedef std::function<bool(T)>	handler_t;
	typedef std::thread		worker_t;

	process_pool(){
	}

	/*
		process_pool

		_initialWorkers : ó���� ������ ������ worker�� ��
		_maxWorker : �ִ� ���� �� �ִ� worker�� ��
		_lifeTime : �� ���� worker�� ���� �� �� �������� Ƚ��
		_handler : workItem�� �ڵ鸵�� �ڵ鷯
	*/
	process_pool( int _initialWorkers,int _maxWorker,
			    int _lifeTime, handler_t _handler) :
		handler( _handler ),
		maxWorker( _maxWorker ), lifeTime( _lifeTime ),
		quit( false ),
		nWorker( 0 ), nWaiting( 0 ), nWorking( 0 ) {

		for(int i=0;i<_initialWorkers;i++)
			add_worker( _lifeTime );
	}
	/*
		~process_pool

		
	*/
	virtual ~process_pool(){
		kill();
	}

	/*
		enqueue

		work queue�� workItem�� ����ִ´�.

		workItem : ���� workItem
	*/
	void enqueue(T workItem){
		// ����ִ� worker�� ���� maxWorker��ŭ worker�� ������
		// �� worker�� �����ϰ� ���� �Ҵ�.
		if( maxWorker < nWorker.load() && nWaiting.load() == 0 ){
			add_worker_with_work( lifeTime, workItem );
		}
		else{
			std::unique_lock<std::mutex> guard( queueMutex );
				qWork.push( workItem );
			guard.unlock();

			// signal�� ��ٸ��� worker�� ���� ���� notify
			if( nWaiting.load() > 0 )
				signal.notify_one();
		}
	}

	/*
		queryPoolStatus

		Ǯ�� ���¸� ���´�.

		waiting : waiting���� worker�� ���� �޾ƿ� ������
		working : working���� worker�� ���� �޾ƿ� ������
	*/
	void query_pool_status(int *waiting,int *working){
		if( waiting != nullptr )
			*waiting = nWaiting.load();
		if( working != nullptr )
			*working = nWorking.load();
	}

	/*
		kill

		��� worker�� ���δ�.
	*/
	void kill(){
		post_quit_workers();

		for( auto &worker : workers )
			worker.detach();
		workers.clear();

		// spin wait
		//   joinable, join ���̿� ���ؽ�Ʈ ����Ī�� �������� ���� ���� �� ���
		//   spin wait�� ����Ѵ�.
		int spinCount = 10000;
		while( nWorker.load() > 0 ){
			if( spinCount )
				spinCount --;
			else{
				std::this_thread::sleep_for(
					std::chrono::milliseconds(1) );
			}
		}
	}

protected:
	/*
		workthread

		worker ������
	*/
	void workthread(int lifeCount){
		nWorker.fetch_add( 1 );

		while( !quit && lifeCount > 0 ){
			T workItem;
			bool result;

			{	
				std::unique_lock<std::mutex> guard( queueMutex );
				
				// double check
				if( qWork.empty() ){
					nWaiting.fetch_add(1);
						signal.wait( guard );
					nWaiting.fetch_sub(1);	

					if( qWork.empty() )
						continue;
				}

				workItem = qWork.front();
				qWork.pop();
			}

			nWorking.fetch_add(1);
				result = handler( workItem );
			nWorking.fetch_sub(1);

			lifeCount --;
		}

		nWorker.fetch_sub( 1 );
	}

	/*
		addWorker

		�� worker�� �߰��Ѵ�.

		lifeCount : ������ī��Ʈ
	*/
	void add_worker(int lifeCount){
		auto boundMethod =
			std::bind( &process_pool::workthread, this, std::placeholders::_1 );

		workers.push_back(
			std::thread( boundMethod, lifeCount ));
	}

	/*
		addWorkerWithWork

		�� worker�� �߰��ϰ� workItem�� �־��ش�.

		lifeCount : ������ī��Ʈ
		workItem : ������ �� �ٷ� ó���� workItem
	*/
	void add_worker_with_work(int lifeCount, T workItem){	
		workers.push_back(
			std::thread( [=](){
				bool result;

				result = handler( workItem );

				workthread( lifeCount );
			}));
	}

	/*
		postQuitWorkers

		��� worker���� ���� ��û�� ������.
	*/
	void post_quit_workers(){
		quit = true;

		signal.notify_all();
	}

protected:
	std::atomic<int> nWorker;	// ������ �� worker�� �� ( nWaiting + nWorking != nWorker )
	std::atomic<int> nWaiting;	// signal �� ��ٸ��� worker�� ��
	std::atomic<int> nWorking;	// handler�� ȣ���Ͽ� ���ϰ� �ִ� worker�� ��

	std::vector<worker_t> workers;	// worker �ν��Ͻ��� ���
	std::queue<T> qWork;	// work queue

	std::condition_variable signal;	// �ñ׳� ��ü
	std::mutex queueMutex;

	handler_t handler;

	int lifeTime;
	int maxWorker;

	bool quit;	// postQuit �÷���
};

#endif //_PROCESS_POOL_H