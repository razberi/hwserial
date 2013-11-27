#include "network.h"
#include "kbhit.h"
//#include <conio.h> // using kbhit.h instead
#include <fstream>
#include <stdio.h>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>

using namespace std;

boost::mutex global_stream_lock;

string hostname = "0.0.0.0";
int port = 8600;

std::string get_file_contents(const char *filename);

class MyConnection : public Connection
{
private:
	void OnAccept( const std::string & host, uint16_t port )
	{
		global_stream_lock.lock();
		std::cout << "[C:" << __FUNCTION__ << "] " << host << ":" << port << std::endl;
		global_stream_lock.unlock();

		// Start the next receive
		Recv();
	}

	void OnConnect( const std::string & host, uint16_t port )
	{
		global_stream_lock.lock();
		std::cout << "[C:" << __FUNCTION__ << "] " << host << ":" << port << std::endl;
		global_stream_lock.unlock();

		// Start the next receive
		Recv();
	}

	void OnSend( const std::vector< uint8_t > & buffer )
	{
		global_stream_lock.lock();
		std::cout << "[C:" << __FUNCTION__ << "] " << buffer.size() << " bytes: "; // << std::endl;
		copy(buffer.begin(), buffer.end(), ostream_iterator<char>(cout, ""));
		cout << endl;
		// for( size_t x = 0; x < buffer.size(); ++x )
		// {
		// 	std::cout << std::hex << std::setfill( '0' ) << 
		// 		std::setw( 2 ) << (int)buffer[ x ] << " ";
		// 	if( ( x + 1 ) % 16 == 0 )
		// 	{
		// 		std::cout << std::endl;
		// 	}
		// }
		// std::cout << std::dec << std::endl;
		global_stream_lock.unlock();
	}

	void OnRecv( std::vector< uint8_t > & buffer )
	{
		global_stream_lock.lock();
		std::cout << "[C:" << __FUNCTION__ << "] " << buffer.size() << " bytes: "; // << std::endl;
		copy(buffer.begin(), buffer.end(), ostream_iterator<char>(cout, ""));
		cout << endl;
		// for( size_t x = 0; x < buffer.size(); ++x )
		// {
		// 	std::cout << std::hex << std::setfill( '0' ) << 
		// 		std::setw( 2 ) << (int)buffer[ x ] << " ";
		// 	if( ( x + 1 ) % 16 == 0 )
		// 	{
		// 		std::cout << std::endl;
		// 	}
		// }
		// std::cout << std::dec << std::endl;
		global_stream_lock.unlock();

		// Start the next receive
		Recv();

		// Echo the data back
		//Send( buffer );

		try {
			std::string jsonData = get_file_contents("jsonData.json");
			std::vector<uint8_t> vJsonData(jsonData.begin(), jsonData.end());
			vJsonData.push_back('\0');
			Send(vJsonData);
		} catch(int ec) {
			global_stream_lock.lock();
			cout << "There was an error while reading the file. Errno: " << ec << endl;
			global_stream_lock.unlock();
		}
	}

	void OnTimer( const boost::posix_time::time_duration & delta )
	{
		//global_stream_lock.lock();
		//std::cout << "[C:" << __FUNCTION__ << "] " << delta << std::endl;
		//global_stream_lock.unlock();
	}

	void OnError( const boost::system::error_code & error )
	{
		global_stream_lock.lock();
		std::cout << "[C:" << __FUNCTION__ << "] " << error << " : " << error.message() << std::endl;
		global_stream_lock.unlock();
	}

public:
	MyConnection( boost::shared_ptr< Hive > hive )
		: Connection( hive )
	{
	}

	~MyConnection()
	{
	}
};

class MyAcceptor : public Acceptor
{
private:
	bool OnAccept( boost::shared_ptr< Connection > connection, const std::string & host, uint16_t port )
	{
		global_stream_lock.lock();
		std::cout << "[A:" << __FUNCTION__ << "] Client connected from: " << host << ":" << port << std::endl;
		global_stream_lock.unlock();

		boost::shared_ptr< MyConnection > nextConnection( new MyConnection( GetHive() ) );
		Accept( nextConnection );

		return true;
	}

	void OnTimer( const boost::posix_time::time_duration & delta )
	{
		//global_stream_lock.lock();
		//std::cout << "[A:" << __FUNCTION__ << "] " << delta << std::endl;
		//global_stream_lock.unlock();
	}

	void OnError( const boost::system::error_code & error )
	{
		global_stream_lock.lock();
		std::cout << "[A:" << __FUNCTION__ << "] " << error << " : " << error.message() << std::endl;
		global_stream_lock.unlock();
	}

public:
	MyAcceptor( boost::shared_ptr< Hive > hive )
		: Acceptor( hive )
	{
	}

	~MyAcceptor()
	{
	}
};

std::string get_file_contents(const char *filename) {
	std::ifstream in(filename, std::ios::in | std::ios::binary);
	if (in) {
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return contents;
	}
	throw(errno);
}

int main( int argc, char * argv[] )
{
	boost::shared_ptr< Hive > hive( new Hive() );

	boost::shared_ptr< MyAcceptor > acceptor( new MyAcceptor( hive ) );
	//acceptor->Listen( "127.0.0.1", 8601 );
	acceptor->Listen( hostname, port );

	global_stream_lock.lock();
	std::cout << "[" << __FUNCTION__ << "] Listening on " << hostname << ":" << port << std::endl;
	global_stream_lock.unlock();

	boost::shared_ptr< MyConnection > connection( new MyConnection( hive ) );
	acceptor->Accept( connection );

	while( !_kbhit() )
	{
		//cout << "Polling..." << endl;
		hive->Poll();
		boost::this_thread::sleep( boost::posix_time::milliseconds(200) );
		//Sleep( 1 );
	}

	hive->Stop();

	return 0;
}