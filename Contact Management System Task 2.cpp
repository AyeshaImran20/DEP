#include<iostream>
#include<fstream>
#include<vector>
#include<string>
using namespace std;

class contact {
public:
	string name;
	string phonenumber;

	contact(string n, string p) {
		name = n;
		phonenumber = p;
	}
};

class contactmanagement {
private:
	vector<contact> contacts;
	const string filename = "contacts.txt";

	void loadcontacts() {
		ifstream file(filename);
		if (file.is_open()) {
			string name, phonenumber;
			while (file >> name >> phonenumber) {
				contacts.push_back(contact(name, phonenumber));
			}
			file.close();
		}
	}

	void savecontact() {
		ofstream file(filename);
		if (file.is_open()) {
			for (const auto& contact : contacts) {
				file << contact.name << " " << contact.phonenumber << endl;
			}
			file.close();
		}
	}

	void clearconsole() {
#ifdef _WIN32
		system("cls");
#else
		system("clear");
#endif 
	}

	void wait() {
		cout << "Press any key to continue..." << endl;
		cin.ignore();
		cin.get();
	}

public:
	contactmanagement() {
		loadcontacts();
	}
	~contactmanagement() {
		savecontact();
	}

	void createcontact() {
		string name, phonenumber;
		cout << " Enter Your Name: " << endl;
		cin >> name;
		cout << " Enter Your Phone Number: " << endl;
		cin >> phonenumber;

		contacts.push_back(contact(name, phonenumber));
		cout << " Contact Created Successfully! " << endl;
		wait();
	}

	void searchcontact() {
		string keyword;
		cout << " Enter Name or Phone Number to Search: " << endl;
		cin >> keyword;
		bool found = false;
		for (const auto& contact : contacts) {
			if (contact.name == keyword || contact.phonenumber == keyword) {
				cout << " Name: " << contact.name << ", Phone Number: "<<contact.phonenumber << endl;
				found = true;
			}
		}
		
		if (!found) {
			cout << " Contact Not found!" << endl;
		}
		wait();
	}

	void viewcontact() {
		if (contacts.empty()) {
			cout << " No Contacts Available." << endl;
		}
		else {
			for (const auto& contact : contacts) {
				cout << " Name: " << contact.name << ", Phone Number: " << contact.phonenumber << endl;

			}
		}
		wait();
	}

	void deletecontact() {
		string keyword;
		cout << " Enter Name or Phone Number to Delete the Contact: " << endl;
		cin >> keyword;
		auto it = remove_if(contacts.begin(), contacts.end(), [&keyword](const contact& contact) {
			return contact.name == keyword || contact.phonenumber == keyword;
			});
		if (it != contacts.end()) {
			contacts.erase(it, contacts.end());
			cout << " Contact Sucessfully Deleted! " << endl;
		}
		else {
			cout << " Contact Not Found! " << endl;
		}
		wait();
	}

	void displaymenu() {
		int choice = 0;
		do {
			clearconsole();

			cout << " *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* " << endl;
			cout << " *        Contact Management System        * " << endl;
			cout << " *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-* " << endl;


			cout << " 1. Create Contact\n";
			cout << " 2. Search Contact\n";
			cout << " 3. View Contacts\n";
			cout << " 4. Delete Contact\n";
			cout << " 5. Exit\n";
			cout << " Enter Your Choice: ";
			cin >> choice;

			switch (choice) {
			case 1:
				createcontact(); break;
			case 2:
				searchcontact(); break;
			case 3:
				viewcontact(); break;
			case 4:
				deletecontact(); break;
			case 5:
				cout << " Exiting..." << endl; break;
			default:
				cout << " Invalid Choice! Please try again. " << endl;
			}
		} while (choice != 5);
	}
};

int main() {
	contactmanagement manager;
	manager.displaymenu();
	return 0;
}