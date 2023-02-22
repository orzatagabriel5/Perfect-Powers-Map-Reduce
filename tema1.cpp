# include "tema1.h"


using namespace std;

pthread_mutex_t mutex;
pthread_barrier_t barrier;


void* reducer(void* args)
{
    // cand mapperi ajung la bariera o sa inceapa si reducerii
    pthread_barrier_wait(&barrier);

    struct arg_reducer arg = *(struct arg_reducer*)args;
    int id = arg.id;
    string output_file;
    output_file.append("out").append(to_string(id+2)).append(".txt");
    
    ofstream out(output_file);
    set<int> res;
    // pentru toti mapperi
    for(int i = 0; i < arg.nr_map; i++){
        // parcurg lista de numere gasite ca puteri perfecte
        for(auto const& map : arg.list->at(i)){
            // pentru puterea aferenta id-ului reducerului
            if(map.first == id + 2){
                set<int> aux = convertToSet(map.second); 
                res.insert(aux.begin(), aux.end());
            }

        }
    }

    out << res.size();
	pthread_exit(NULL);
}

void* mapper(void* args)
{

    struct arg_mapper arg = *(struct arg_mapper*)args;
    string file;
    int x, nr, thread_id = arg.id;

    while(!(arg.file_list->empty())){

        pthread_mutex_lock(&mutex);
        file = arg.file_list->back();
        arg.file_list->pop_back();
        pthread_mutex_unlock(&mutex);

        ifstream in(file);
        in >> x;
        for(int i = 0; i < x; i++){
            in >> nr;
            if(nr == 1){
            }
            // functie returneaza o lista cu exponentii numarului,
            // daca exista
            vector<int> r = perfect_power(nr, arg.nr_reduce);
            
            if(!r.empty()){
                // adaug fiecare exponent in lista finala a mapperului
                for(auto const& x : r){
                    if(arg.list->at(thread_id).find(x) == arg.list->at(thread_id).end()){
                        arg.list->at(thread_id).insert({x, {nr}});
                    } else {
                        arg.list->at(thread_id).at(x).push_back(nr);
                    }
                }
            } 
        }
    }
    // cand mapperi ajung la bariera vor incepe si reducerii
    pthread_barrier_wait(&barrier);

	pthread_exit(NULL);
}


int main (int argc, char **argv)
{
    int r;
	void *status;
    int nr_map = atoi(argv[1]);
    int nr_reduce = atoi(argv[2]);

    std::vector<string> file_list;
    // lista finala cu puteri perfecte a tuturor mapperilor
    std::vector<std::map<int, std::vector<int>>> list;
    list.resize(nr_map);

    ifstream in(argv[3]);
    int nr_files;
    in >> nr_files;

    // citesc toate fisierele din fisierul dat ca input
    for(int i = 0; i < nr_files; i++){
        string name;
        in >> name;
        file_list.push_back(name);
    }

    // fac o structura de date pentru fiecare thread in parte
    vector<struct arg_mapper> map_args;
    map_args.resize(nr_map);

    vector<struct arg_reducer> reduce_args;
    reduce_args.resize(nr_reduce);

	pthread_t threads[nr_map + nr_reduce];
    pthread_mutex_init(&mutex, NULL);
    pthread_barrier_init(&barrier, NULL, (nr_map + nr_reduce));

    for (int i = 0; i < (nr_map + nr_reduce); i++) {
        if(i < nr_map){
            map_args[i].id = i;
            map_args[i].file_list = &file_list;
            map_args[i].list = &list;
            map_args[i].nr_reduce = nr_reduce;
		    r = pthread_create(&threads[i], NULL, mapper, &map_args[i]);
        } else {
            int x = i - nr_map;
            reduce_args[x].id = i - nr_map;
            reduce_args[x].list = &list;
            reduce_args[x].nr_map = nr_map;
		    r = pthread_create(&threads[i], NULL, reducer, &reduce_args[x]);
        }

		if (r) {
			printf("Eroare la crearea thread-ului %d\n", i);
			exit(-1);
		}
	}

	for (int i = 0; i < (nr_map + nr_reduce); i++) {

		r = pthread_join(threads[i], &status);
		if (r) {
			printf("Eroare la asteptarea thread-ului %d\n", i);
			exit(-1);
		}
	}

    pthread_mutex_destroy(&mutex);
    pthread_barrier_destroy(&barrier);

    return 0;
}