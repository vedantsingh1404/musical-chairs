/*
 * Program: Musical chairs game with n players and m intervals.
 * Authors: Vedant Singh, Shreyas Jayant Havaldar
 * Roll# : CS18BTECH11047, CS18BTECH11042
 */

#include <stdlib.h>  /* for exit, atoi */
#include <iostream>  /* for fprintf */
#include <errno.h>   /* for error code eg. E2BIG */
#include <getopt.h>  /* for getopt */
#include <assert.h>  /* for assert */
#include <chrono>	/* for timers */
#include <mutex>
#include <condition_variable>
#include <thread>
using namespace std;

int nplayers;
int ready_count = 0, end_count = 0;
int dead;
int player_count=0;
int num_chairs;

mutex music_start, music_end, player, creation, mus, count_mutex, locking;
mutex l_s_mutex, m_s_mutex, m_e_mutex, l_e_mutex;
//unique_lock<mutex> l_s_lck(l_s_mutex);
//unique_lock<mutex> m_s_lck(m_s_mutex);
//unique_lock<mutex> m_e_lck(m_e_mutex);
//unique_lock<mutex> l_e_lck(l_e_mutex);
condition_variable l_s, m_s, m_e, l_e;

bool *chair_array;
bool *isalive;

mutex *chair;

std::condition_variable cv;
std::mutex cv_m; // This mutex is used for three purposes:
                 // 1) to synchronize accesses to i
                 // 2) to synchronize accesses to std::cerr
                 // 3) for the condition variable cv
int i = 0;
 
void waits()
{
    std::unique_lock<std::mutex> lk(l_s_mutex);
    std::cerr << "Waiting... \n";
    cv.wait(lk, []{return i == 1;});
    std::cerr << "...finished waiting. i == 1\n";
}
 
void signals()
{
 
    {
        std::lock_guard<std::mutex> lk(l_s_mutex);
        i = 1;
        std::cerr << "Notifying again...\n";
    }
    cv.notify_all();
}

void usage(int argc, char *argv[]);
unsigned long long musical_chairs();

using namespace std;

int main(int argc, char *argv[])
{
    int c;
	//int nplayers = 0;

    while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
            {"help",            no_argument,        0, 'h'},
            {"nplayers",         required_argument,    0, '1'},
            {0,        0,            0,  0 }
        };

        c = getopt_long(argc, argv, "h1:", long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
        case 0:
            cerr << "option " << long_options[option_index].name;
            if (optarg)
                cerr << " with arg " << optarg << endl;
            break;

        case '1':
            nplayers = atoi(optarg);
            break;

        case 'h':
        case '?':
            usage(argc, argv);

        default:
            cerr << "?? getopt returned character code 0%o ??n" << c << endl;
            usage(argc, argv);
        }
    }

    if (optind != argc) {
        cerr << "Unexpected arguments.\n";
        usage(argc, argv);
    }


	if (nplayers == 0) {
		cerr << "Invalid nplayers argument." << endl;
		return EXIT_FAILURE;
	}

    chair = (mutex*)malloc((nplayers - 1) * sizeof(mutex));
    isalive = (bool*)malloc(nplayers * sizeof(bool));
    chair_array = (bool*)malloc((nplayers - 1) * sizeof(bool));
    for(int i = 0;i < nplayers - 1;i++) {
        chair_array[i] = false;
    }
    for(int i = 0;i < nplayers;i++) {
        isalive[i] = true;
    }

    num_chairs = nplayers - 1;

    unsigned long long game_time;
	game_time = musical_chairs();

    cout << "Time taken for the game: " << game_time << " us" << endl;

    exit(EXIT_SUCCESS);
}

void usage(int argc, char *argv[])
{
    cerr << "Usage:\n";
    cerr << argv[0] << "--nplayers <n>" << endl;
    exit(EXIT_FAILURE);
}

void umpire_main() {

    cout << "umpire \n";
    string command;
    while(nplayers > 1) {    
        cout << nplayers << "\n";
        cin >> command;
        if (command == "ls") {
        }
        else if(command == "ms") {
        }
        else if(command == "mst") {
        }
        else if(command == "lst") {
        }
    }

    if(nplayers == 1) {
        for(int i = 0;i < nplayers;i++) {
            if (isalive[i] == true) {
                cout << "Winner :" << i << "\n";
                break;
            }
        }
    }

    return;
}

void player_main(int plid) {
    bool alive = true;
    while(alive) {
        int i = rand() % num_chairs;
        int j = i;
        do{
            if (chair[j].try_lock()) {
                if(chair_array[j] == false) {
                    chair_array[j] = true;
                    cout << j << "\n";
                    alive = true;
                    break;
                }
                chair[j].unlock();
            }
            j = (j + 1) % num_chairs;
        }while(j != i);

        if (j == i) {
            alive = false;
            dead = plid;
            isalive[plid] = false;
            nplayers--;
            num_chairs--;
            for(int i = 0;i < nplayers - 1;i++) {
                chair_array[i] = false;
            }
        }
    }
    return;
}

unsigned long long musical_chairs()
{
	auto t1 = chrono::steady_clock::now();

    thread umpire;
    umpire = thread(umpire_main);

	// Spawn umpire thread.
    /* Add your code here */
    thread players[nplayers];
    int id[nplayers];
    for(int i = 0;i < nplayers;i++) {
        id[i] = i;
    }
    for (int i=nplayers - 1; i >= 0; i--) {
        players[i] = thread(player_main, i);
    }
    for (int i=0; i<nplayers; i++) {
        players[i].join();
    };
	// Spawn n player threads.
    //umpire.join();
    /* Add your code here */

	auto t2 = chrono::steady_clock::now();

	auto d1 = chrono::duration_cast<chrono::microseconds>(t2 - t1);

	return d1.count();
} 
