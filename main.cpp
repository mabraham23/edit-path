#include "iostream"
#include "fstream"
#include <string>
#include <vector>

int diff( std::string word_1, std::string word_2, int i, int j ) {
    if ( word_1[i] == word_2[j]) {
        return 0;
    }
    else {
        return 1;
    }
}

int edit_distance( std::string word_1, std::string word_2 ) {
    std::vector<std::vector<int>> table(word_1.length()+1);

    for( int i=0; i<word_1.length()+1; i++ ) {
        table[i].resize(word_2.length()+1);
    }
     
    // initialize the first column and row of the table
    for( int i=0; i<word_1.length()+1; i++ ){
        table[i][0] = i;
    };
    for( int j=1; j<word_2.length()+1; j++ ){
        table[0][j] = j;
    };
    // calculate the edit distance for table entry
    for( int i=1; i<word_1.length()+1; i++ ) {
        for( int j=1; j<word_2.length()+1; j++ ) {
            table[i][j] = std::min({((table[i-1][j])+1), ((table[i][j-1])+1), ((table[i-1][j-1])+diff(word_1, word_2, i-1, j-1))});
        }
    }

    std::cout << "\n";
    for( int i=0; i<word_1.length()+1; i++ ) {
        for( int j=0; j<word_2.length()+1; j++ ) {
            if ( (table[i][j] > 9 )) {
                std::cout << " " << table[i][j];
            }
            else {
                std::cout << " " << table[i][j] << " ";
            }
        }
        std::cout << "\n";
    }

    return table[word_1.length()][word_2.length()];
}

bool compareFileSize(std::string f1, std::string f2) {
    int endIdx = f1.rfind('-');
    int beginIdx = f1.rfind('-', endIdx - 1);
    int size1 = atoi(f1.substr(beginIdx + 1, endIdx - beginIdx - 1).c_str());

    endIdx = f2.rfind('-');
    beginIdx = f2.rfind('-', endIdx - 1);
    int size2 = atoi(f2.substr(beginIdx + 1, endIdx - beginIdx - 1).c_str());

    return size1 < size2;
}

void read_files( std::vector<std::string> files ) {
    std::ofstream t("output/prim-timing.csv");
	std::ofstream p("output/prim-timing-graphs.csv");

	double size_avg_time =0;
	int size_avg_reps = 0;
	int prev_graph_size = 10;

	for ( auto file: files ) {
		double avgTime = 0;
		double reps = 0;
		int totalCost = 0;

		int lastDash = file.rfind('-');
		int firstDash = file.rfind('-', lastDash - 1);
		int graphSize = atoi(file.substr(firstDash + 1, lastDash - firstDash - 1).c_str());

		std::ifstream f;
		f.open(file);
		if (!f.good()) {
			std::cerr << "Error reading file: " << file << std::endl;
			exit(EXIT_FAILURE);
		}
		std::string line;

		// First line is the number of Vertices
		while (getline(f, line)) {
			if (line.at(0) == '#')
				continue;
			else
				break;
		}

		int V = std::stoi(line);
		struct Graph* graph = createGraph(V); 
		while (getline(f, line)) {
            std::string word_1, word_2;
			int v1, v2, weight;
			// std::istringstream ss(line);
            ss >> word_1 >> word_2;
			ss >> v1 >> v2 >> weight;
			v1 --;
			v2 --;
			addEdge(graph, v1, v2, weight); 
		}
		f.close();

		int testreps = 1;
		if ( graphSize <= 100000 ) {
			testreps = 1;
		}
		if ( graphSize <= 10000 ) {
			testreps = 1;
		}
		if ( graphSize <= 1000 ) {
			testreps = 1;
		}
		for ( size_t i = 0; i < testreps; i++ ) {
			reps ++;
			double t1, t2;
			t1 = clock();
			totalCost = PrimMST(graph); 
			t2 = (clock() - t1 ) / CLOCKS_PER_SEC;
			avgTime += t2;
		}

		double graphTime = 0;
		graphTime = avgTime / reps;

		if ( prev_graph_size == graphSize ) {
			size_avg_time += graphTime;
			size_avg_reps += 1;
			if ( file == "graphs/graph-800000-4.txt" ) {
				std::cout << graphSize << ", " << size_avg_time / size_avg_reps << std::endl;
				t << graphSize << ", " << size_avg_time / size_avg_reps << "\n";
			}
		}
		else {
			std::cout << prev_graph_size << ", " << size_avg_time / size_avg_reps << std::endl;
			t << prev_graph_size << ", " << size_avg_time / size_avg_reps << "\n";
			prev_graph_size = graphSize;
			size_avg_time = graphTime;
			size_avg_reps = 1;
		}
		p << file << ", " << avgTime / reps << ", " << totalCost << std::endl;
	}
	t.close();
	p.close();
}

int main( int argc, const char* argv[] ) {

    // read in the word files
    std::vector<std::string> files;
	for( int i = 1; i < argc; i++ ){
        files.push_back(argv[i]);
    }

    std::sort(files.begin(), files.end(), compareFileSize);

    read_files(files);

    // read in the words to be processed
    std::string word_1;
    std::string word_2;
    word_1 = argv[1];
    word_2 = argv[2];
    int cost = edit_distance(word_1, word_2);
    std::cout << "\nMinimum edit cost is " << cost << std::endl << "\n";
    return 0;
}