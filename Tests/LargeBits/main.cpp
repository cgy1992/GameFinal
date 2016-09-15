#include <iostream>
#include <string>
#include <sstream>
#include <large_bits.h>
#include <Vector3.h>

using namespace std;
using namespace gf;

int main()
{
	large_bits_128 b[5];
	string command;
	int pos;
	int i, j;

	while (cin >> command) {
		if (command == "set") {
			cin >> i >> pos;
			b[i].set(pos);
		}
		else if (command == "unset") {
			cin >> i >> pos;
			b[i].unset(pos);
		}
		else if (command == "isset") {
			cin >> i >> pos;
			cout << b[i].is_set(pos) << endl;
		}
		else if (command == "==") {
			cin >> i >> j;
			cout << (b[i] == b[j]) << endl;
		}
		else if (command == "!=") {
			cin >> i >> j;
			cout << (b[i] != b[j]) << endl;
		}
		else if (command == "print") {
			cin >> i;
			cout << b[i].toString() << endl;
		}
		else if (command == "=") {
			cin >> i >> j;
			b[i] = b[j];
		}
		else if (command == "<") {
			cin >> i >> j;
			cout << (b[i] < b[j]) << endl;
		}
		else if (command == "<=") {
			cin >> i >> j;
			cout << (b[i] <= b[j]) << endl;
		}
		else if (command == ">") {
			cin >> i >> j;
			cout << (b[i] > b[j]) << endl;
		}
		else if (command == ">=") {
			cin >> i >> j;
			cout << (b[i] >= b[j]) << endl;
		}
	}


	return 0;
}