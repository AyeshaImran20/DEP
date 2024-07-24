#include<iostream>
#include<fstream>
#include<string>

using namespace std;

string filereading(const string &filepath) {
	ifstream file(filepath, ios::in | ios::binary);
	if (!file) {
		throw runtime_error("could not open file for reading");
	}
	string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
	return content;
}

void writefile(const string &filepath, const string &data) {
	ofstream file(filepath, ios::out | ios::binary);
	if (!file) {
		throw runtime_error("could not open file for writing");
	}
	file.write(data.c_str(), data.size());
}

string compressRLE(const string &data) {
	string compressed;
	int n = data.size();
	for (int i = 0; i< n; ++i) {
		int count = 1;
		while (i + 1<n&&data[i] == data[i + 1]) {
			++i;
			++count;
		}
		compressed += data[i];
		compressed += to_string(count);
	}
	return compressed;
}

string decompressRLE(const string &data) {
	string decompressed;
	int n = data.size();
	for (int i = 0; i<n; ++i) {
		char ch = data[i];
		string countStr;
		while (i + 1<n && isdigit(data[i = 1])) {
			countStr += data[++i];
		}
		int count = stoi(countStr);
		decompressed.append(count, ch);
	}
	return decompressed;
}

int main() {
	string inputfilepath, outputfilepath;
	char choice;

	cout << "Enter the path of the input file:" << endl;
	cin >> inputfilepath;

	cout << "choose operation: C. compress or D. decompress:" << endl;
	cin >> choice;

	if (choice == 'C' || choice == 'c') {
		try {
			string data = filereading(inputfilepath);
			string compressedData = compressRLE(data);
			outputfilepath = inputfilepath + ".rle";
			writefile(outputfilepath, compressedData);
			cout << "file compressed successfully. Output file:" << outputfilepath << endl;
		}
		catch (const exception &e) {
			cerr << "Error:" << e.what() << endl;
		}

	}

	else if (choice == 'D' || choice == 'd') {
		try {
			string data = filereading(inputfilepath);
			string decompressedData = decompressRLE(data);
			outputfilepath = inputfilepath + ".decompressed";
			writefile(outputfilepath, decompressedData);
			cout << "File decompressed succesfully. Output file:" << outputfilepath << endl;
		}
		catch (const exception&e) {
			cerr << "Error:" << e.what() << endl;
		}

	}
	else {
		cout << "Invalid choice. Please enter 'c'for compress or 'd' for decompress." << endl;
	}

	return 0;
}
