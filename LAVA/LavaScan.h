#pragma once
#ifndef LAVASCAN_H
#define LAVASCAN_H
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS
extern std::string CurrentScanFile = std::string("");

class ProgressMonitor {

	/*
		How to use this class:
		A. Declare it before a scan. For each directory you want to scan:

			1. Run the function, Reccommend(std::string directory);
				- This will return one of the following int values:
					- 0 if it's reccomended that the progress is monitored by counting files (this includes all files recursively)
					- 1 if it's reccomended that the progress is moinitored by counting level 5 folders (i.e. directories found at level four recursively if the parent folder is 0)

			2. To determine the total progress necessary, for each directory:
				- Run either:
					- CountDirectories(std::string directory);
					- CountFiles(std::string directory);
				- Note: while the reccommend function will make a reccommendation, it is only that- a reccommendation.
				        With this in mind, you may choose to ignore it, so ultimately the choice is yours how you'd like to proceed as long as you keep it consistant.

			3. From there every time you finish scanning a file or level 5 folder:
				- Run the respective function depending on which it was:
					- FinishedDirectory();
					- FinishedFile();
					- FinishedFile();
				- The internal varibales will automatically update accordingly.
		
		B.  To return the current progress, you have a few options:
			- GetPercentage();
				- Returns a float of the percentage of progress completed (range 0.0 - 100.0)
			- GetTotal();
				- Returns a double of the total amount of notable files/folders to scan (both completed and remaining)
			- GetCompleted();
				- Returns a double of the total amount of notable files/folders that have already been scanned

	*/

protected:
	//Internal variables
	//Folders Scanned
	double totalFolderCount = 0;
	double folderCountCompleted = 0;
	//Files Scanned
	double totalFileCount = 0;
	double fileCountCompleted = 0;
	//Total Progress
	double totalProgressCount = 0;
	double totalProgressCompleted = 0;
	//1-100% value for the progress bar
	float progessPercentage = 0;

	//Internal function to keep the variables up to date
	void UpdatePercentage()
	{
		if (totalProgressCount != 0) {
			progessPercentage = totalProgressCompleted / totalProgressCount;
		}
		else {
			progessPercentage = 0;
		}
		if (progessPercentage > 100){
			progessPercentage = 100;
		}
		return;
	}

public:
	//Return the variables for use outside of the class
	float GetPercentage(){
		return progessPercentage;
	}
	double GetTotal() {
		return totalProgressCount;
	}
	double GetCompleted() {
		return totalProgressCompleted;
	}

	//Only run this if the scan is finished and the progress monitor falls behind. This cannot be undone.
	void FinishedEarly() {
		totalProgressCompleted = totalProgressCount;
		progessPercentage = 100;
		return;
	}

	//Resets everything back to default. This cannot be undone.
	void ResetProgressMonitor() {
	    totalFolderCount = 0;
		folderCountCompleted = 0;
		totalFileCount = 0;
		fileCountCompleted = 0;
		totalProgressCount = 0;
		totalProgressCompleted = 0;
		progessPercentage = 0;
		return;
	}

	//Call this every time a level five folder is scanned during a scan
	void FinishedDirectory(){
		folderCountCompleted++;
		UpdatePercentage();
		return;
	}
	//Call this every time a notable file is scanned during a scan
	void FinishedFile() {
		fileCountCompleted++;
		UpdatePercentage();
		return;
	}

	//Adds a single file to the total to scan. For if the user adds a single file in advanced scan.
	void AddFile() {
		totalFileCount++;
		return;
	}
	
	//Call this for each directory added to a scan
	//Adds the amount of level five directories within a given directory to the total count
	void CountDirectories(std::string directory, int depth = 4, int level = 0) {
		//Source used: https://github.com/tronkko/dirent/blob/master/examples/ls.c
		//This is a modified version of iterateDirectory
		if (directory.substr(directory.length() - 2) == ".\\")
		{
			return;
		}
		struct dirent* item;
		const char* location = directory.c_str();
		DIR* dir = opendir(location);
		if (dir != NULL) {
			item = readdir(dir);
			while (item != NULL) {
				if (item->d_type == DT_DIR)
				{
					//If we're not at the fifth level yet, go a level deeper
					if (level < depth)
					{
						std::string newDirectory = directory + item->d_name + "\\";
						CountDirectories(newDirectory, depth, level++);
					}
					//If we are then add the amount of folders to the total count
					if (level == 4)
					{
						totalFolderCount++;
					}
				}
				item = readdir(dir);
			}
		}
		closedir(dir);
		return;
	}

	void CountFiles(std::string directory) {
		//Source used: https://github.com/tronkko/dirent/blob/master/examples/ls.c
		//This is a modified version of iterateDirectory
		if (directory.substr(directory.length() - 2) == ".\\")
		{
			return;
		}
		struct dirent* item;
		const char* location = directory.c_str();
		DIR* dir = opendir(location);
		if (dir != NULL) {
			item = readdir(dir);
			while (item != NULL) {
				if (item->d_type == DT_DIR)
				{
					std::string newDirectory = directory + item->d_name + "\\";
					CountDirectories(newDirectory);
				}
				if (item->d_type == DT_REG)
				{
					totalFolderCount++;
				}
				item = readdir(dir);
			}
		}
		closedir(dir);
		return;
	}

	int Reccommend(std::string directory, int depth = 4, int level = 0)
	{
		int procedure = 0;
		//Source used: https://github.com/tronkko/dirent/blob/master/examples/ls.c
		//This is a modified version of iterateDirectory
		if (directory.substr(directory.length() - 2) == ".\\")
		{
			return 0;
		}
		struct dirent* item;
		const char* location = directory.c_str();
		DIR* dir = opendir(location);
		if (dir != NULL) {
			item = readdir(dir);
			while (item != NULL) {
				if (item->d_type == DT_DIR)
				{
					//If we're not at the fifth level yet, go a level deeper
					if ((level < depth) && (procedure == 0))
					{
						std::string newDirectory = directory + item->d_name + "\\";
						procedure = Reccommend(newDirectory, depth, level++);
					}
					//If we are then add the amount of folders to the total count
					if (level == depth)
					{
						return 1;
					}
				}
				item = readdir(dir);
			}
		}
		closedir(dir);
		return procedure;
	}
	
};


class LavaScan
{
public:
	// memburs
	std::string AntibodyFileLocation = "test.LavaAnti";
	int fd, ret, CurrentScanCount;
	unsigned long int size = 0; std::string start_time; std::string finish_time;
	unsigned int sigs = 0;
	long double mb;
	const char* virname;
	struct cl_engine* engine;
	struct cl_scan_options options;
	bool isScanDone;
	struct q_entry;
	std::string currentScanGoing;
	std::vector<std::vector<std::string>> PreviousScans;
	std::set<LavaScan::q_entry> QuarantineContents;
	int num_found;
	int num_removed;
	// constructor
	LavaScan(); // default
	/* scans */
	bool CompleteScan();
	bool QuickScan();
	bool AdvanceScan();
	bool AdvanceScanNow(std::set<std::string> s);
	void eicarTest(std::string path="C:\\test\\eicar.com.txt");
	std::set<LavaScan::q_entry> read_quarantine_contents();
	void quarantine_file(std::string filepath, std::string virus_name);
	bool remove_quarantined_files(std::set<std::string> to_remove);
	bool postMoveQuarantine(std::set<q_entry> to_remove);
	bool quarantineIsEmpty();
	bool quarantine_contains(std::string file_name);
	void move_file(std::string file_path, std::string new_location_with_new_filename);
	void make_quarantine_directory();
	bool scanDirectory(std::string dirPath);
	void iterateDirectory(std::string directory, bool clean);
	int scanFile(std::string filePath);
	void SupportPage();
	int scheduleScanWeekly(int inputDay, int inputHour, int inputMinute);
	int scheduleScanMonthly(int inputDay, int inputHour, int inputMinute);
	int rmScheduledScan();
	void AddToAntibody(std::string dirPath, std::string antibodyfilelocation);
	bool reset_QC();
	std::vector<std::string> ReadAntibody(std::string antibodyfilelocation);
	int FileCount(std::string dirPath);
	int TotalSetFileCount(std::set<std::string> p);
	std::set<std::string> countQuarantineContents();
	void log_scan();
	std::vector<std::vector<std::string>> read_log();
	std::string get_time();
	void UpdatePreviousScans();
	bool moveQuarantineHome(std::set<q_entry> q);
};

//Open support page in the default browser
inline void LavaScan::SupportPage() {
	//The shell command is marginally slower but will use the default browser. It also requires shellapi.h, which I've included.
	ShellExecute(0, 0, L"https://github.com/kornfeldm/LAVA/issues", 0, 0, SW_SHOW);
	//The system command below is faster but cannot use default browser so I commented it out. If performance becomes a problem use this instead.
	//Because I had to choose a browser I chose the one guaranteed to be installed.
	//system("start microsoft-edge:https://github.com/kornfeldm/LAVA/issues");
	
}

//Make a monthly scan. Parameters -> inputDay: Day of Week (1-7, where 1 is Sunday, 7 is Saturday); inputHour: Hour of Day (0-23); inputMinute: Minute of Hour (0-59);
//Returns 0 on completion or errors -1, -2 or -3 if there's an issue with the parameters depending on which causes the problem
inline int LavaScan::scheduleScanWeekly(int inputDay, int inputHour, int inputMinute) {
	//Yes it's easier to just use strings, but the system command can be attacked with an injection attack if not used carefully
	std::string day = "Sun";
	std::string time = "00:00";
	std::string hour = "00";
	std::string minute = "00";
	switch (inputDay) {
	case 1:
		day = "SUN";
		break;
	case 2:
		day = "MON";
		break;
	case 3:
		day = "TUE";
		break;
	case 4:
		day = "WED";
		break;
	case 5:
		day = "THU";
		break;
	case 6:
		day = "FRI";
		break;
	case 7:
		day = "SAT";
		break;
	default:
		return -1;
		break;
	}

	if (inputHour < 0 || inputHour > 23){
		return -2;
	}
	if (inputHour < 10){
		hour = "0" + std::to_string(inputHour);
	} else {
		hour = std::to_string(inputHour);
	}

	if (inputMinute < 0 || inputMinute > 59) {
		return -3;
	}
	if (inputMinute < 10) {
		minute = "0" + std::to_string(inputMinute);
	}
	else {
		minute = std::to_string(inputMinute);
	}

	time = hour + ":" + minute;

	std::string path = "\"C:\\Windows\\System32\\notepad.exe\""; //By launching this executable
	std::string cmdcpp = "SCHTASKS /CREATE /SC Weekly /D " + day + " /TN \"LAVA\\ScheduleScan\" /TR " + path + " /ST " + time;
	system(cmdcpp.c_str());
	return 0;
}

//Make a monthly scan. Parameters -> inputDay: Day of Month (1-31); inputHour: Hour of Day (0-23); inputMinute: Minute of Hour (0-59);
//Returns 0 on completion or errors -1, -2 or -3 if there's an issue with the parameters depending on which causes the problem
inline int LavaScan::scheduleScanMonthly(int inputDay, int inputHour, int inputMinute) {
	//Yes it's easier to just use strings, but the system command can be attacked with an injection attack if not used carefully
	std::string day = "1";
	std::string time = "00:00"; 
	std::string hour = "00";
	std::string minute = "00";
	if (inputDay < 1 || inputDay > 31) {
		return -1;
	} else {
		day = std::to_string(inputDay);
	}

	if (inputHour < 0 || inputHour > 23) {
		return -2;
	}

	if (inputHour < 10) {
		hour = "0" + std::to_string(inputHour);
	} else {
		hour = std::to_string(inputHour);
	}

	if (inputMinute < 0 || inputMinute > 59) {
		return -3;
	}

	if (inputMinute < 10) {
		minute = "0" + std::to_string(inputMinute);
	} else {
		minute = std::to_string(inputMinute);
	}

	time = hour + ":" + minute;

	std::string path = "\"C:\\Windows\\System32\\notepad.exe\""; //By launching this executable
	std::string cmdcpp = "SCHTASKS /CREATE /SC Monthly /D " + day + " /TN \"LAVA\\ScheduleScan\" /TR " + path + " /ST " + time;
	system(cmdcpp.c_str());
	return 0;
}

//Removes the scheduled scan and returns 0 on completion
inline int LavaScan::rmScheduledScan() {
	std::string cmdcpp = "SCHTASKS /DELETE /TN \"LAVA\\ScheduleScan\"";
	system(cmdcpp.c_str());
	return 0;
}

inline int LavaScan::scanFile(std::string filePath) {
	// update scan count
	CurrentScanCount++;
	// update current scan dir for GUI
	CurrentScanFile = filePath;
	//std::cout << "\n\tfile:" << filePath;
	const char* virname;
	int ret = cl_scanfile(filePath.c_str(), &virname, NULL, engine, &options); //scanning file using clamAV
	if (ret == CL_VIRUS) {
		/*printf("--------------------------------------------------------------------------------------\n");
		printf("virus detected: %s\n", virname);
		getch();
		printf("--------------------------------------------------------------------------------------\n");*/
		//QUARANTINE FILE IF NOT IN SYSTEM FOLDER
		if (filePath.substr(3, 7) == "Windows")
		{
			printf("VIRUS DETECTED IN SYSTEM FOLDER! FILE %s IS INFECTED! IMMIDIATE ACTION REQUIRED!", filePath); //Infected file is in system folder
		}
		else {
			quarantine_file(filePath, virname); //Infected file is not in system folder
		}
	}
	else {
		//printf("No virus detected.\n");
		if (ret != CL_CLEAN) {
			printf("Error: %s\n", cl_strerror(ret)); //In case of scan error
		}
	}
	return ret; //returns scan output value: either CL_VIRUS, CL_CLEAN or CL_ERROR
}

inline void LavaScan::iterateDirectory(std::string directory, bool clean)
{
	//Source used: https://github.com/tronkko/dirent/blob/master/examples/ls.c

	//Prevents an infinite loop since ./ comes up as a folder for each directory
	if (directory.substr(directory.length() - 2) == ".\\")
	{
		return;
	}


	//If this is a subdirectory of C:/Windows then eleveate the mode to 1
	//if (directory.substr(3, 7) == "Windows")
	//{
	//    mode = 1;
	//}


	//Initilize the dirent class
	struct dirent* item;
	const char* location = directory.c_str();
	DIR* dir = opendir(location);

	//If the directory is value
	if (dir != NULL)
	{
		//Set "item" to the first item in the directory
		item = readdir(dir);
		//It it's valid (i.e. not the end of the directory)
		while (item != NULL)
		{
			//If the item is a folder
			if (item->d_type == DT_DIR)
			{
				//Check all the subdirectories in it
				std::string newDirectory = directory + item->d_name + "\\";

				//std::cout << newDirectory << "\n";
				iterateDirectory(newDirectory, clean);
				//CHECK FOR USER CANCEL GOES HERE
				//closedir(directory)
				//return

			}
			//If the item is a file
			if (item->d_type == DT_REG)
			{
				std::string filePath = directory + item->d_name; // generate filepath

				//std::cout << filePath << "\n";
				//scan the file using clamAV

				if (scanFile(filePath) == CL_VIRUS) {
					clean = false; //File in direcotry is infected
					this->num_found++;//increase the virus count
					////this->viruses_found++;//increase the virus count
				}
				//CHECK FOR USER CANCEL GOES HERE
				//closedir(directory)
				//return
			}
			//Shift the item to look at the next item
			item = readdir(dir);

		}
	}
	//Close the directory. Will cause temporary write issues if this does not happen.
	//In its current state, do not interrupt the process. We will need to fix this before release
	closedir(dir);
}

//Read each line in the specified file and output a string vector containing each directory
inline std::vector<std::string> LavaScan::ReadAntibody(std::string antibodyfilelocation)
{
	std::vector<std::string> directorylist;
	std::ifstream antibody;
	std::string listedirectory;
	antibody.open(antibodyfilelocation);
	if (antibody.is_open())
	{
		while (!antibody.eof())
		{
			getline(antibody, listedirectory);
			directorylist.push_back(listedirectory);
			std::cout << listedirectory << "\n";
		}
		antibody.close();
		return directorylist;
	}
	else
	{
		//Error opening file
	}
	return directorylist;
}

namespace fs = std::filesystem;

inline int LavaScan::FileCount( std::string dirPath )
{
	getch();
	int count = 0;
	for (auto& p : fs::recursive_directory_iterator(dirPath))
		count++;
	
	return count;
}

inline int LavaScan::TotalSetFileCount(std::set<std::string> p) {
	int count = 0;
	for (auto path : p) {
		//std::cout << "\t" << path << ".\n";
		struct stat s;
		if (stat(path.c_str(), &s) == 0)
		{
			if (s.st_mode & S_IFDIR)
			{
				count += FileCount(path);
			}
			else if (s.st_mode & S_IFREG)
			{
				count++;
			}
			else
			{
				//something else
				std::cout << "\n ok wtf is this\n";
			}
		}
		else
		{
			//error
			return -4;
		}
	}

	return count;
}

//Write new directories to the antibody file. Parameters are <vector containting strings of diretories>, path to antibody file
void WriteAntibody(std::vector<std::string> directorylist, std::string antibodyfilelocation)
{
	std::ofstream antibody;
	antibody.open(antibodyfilelocation);
	if (antibody.is_open())
	{
		for (int i = 0; i < directorylist.size(); i++)
		{
			antibody << directorylist[i] << std::endl;
		}
		antibody.close();
	}
}

//Check if a directory is in the antibody file. Add it if it isn't.
//Ignore it if it's a subdirectory.
//Replace any subdirectories
inline void LavaScan::AddToAntibody(std::string dirPath, std::string antibodyfilelocation)
{
	std::vector<std::string> existing = ReadAntibody(antibodyfilelocation);
	bool newDirectory = true;
	int replace = 0;
	for (int i = 0; i < existing.size(); i++)
	{
		if (strstr(existing[i].c_str(), dirPath.c_str()) != NULL)
		{
			newDirectory = false;
		}
		if (strstr(dirPath.c_str(), existing[i].c_str()) != NULL)
		{
			existing[i] = dirPath;
		}
	}
	//Check for duplicates --This code should be tested more, it's just a temporary fix.
	for (int i = 0; i < existing.size(); i++)
	{
		for (int j = 0; j < existing.size(); j++)
		{
			if (existing[i] == existing[j])
			{
				existing.erase(existing.begin() + i);
			}
		}
	}

	if (newDirectory == true)
	{
		std::vector<std::string> directoryToAdd;
		directoryToAdd.push_back(dirPath);
		WriteAntibody(directoryToAdd, antibodyfilelocation);
	}
	remove(antibodyfilelocation.c_str());
	WriteAntibody(existing, antibodyfilelocation);
}

inline bool LavaScan::reset_QC() { //after a scan just reset quarantine contents
	this->QuarantineContents.clear();
	this->QuarantineContents = read_quarantine_contents();
	CurrentScanCount = 0;
	currentScanGoing = "";
	num_found = 0; num_removed = 0;
	isScanDone = false;
	start_time = std::string("");
	finish_time = std::string("");
	return true;
}

inline bool LavaScan::scanDirectory(std::string dirPath) {
	this->CurrentScanCount++; // 
	bool clean = true; //Setting directory as clean
	bool cancel = false; //A cancel variable in case the user wants to cancel the scan
	iterateDirectory(dirPath, clean);//call recursive helper funciton
	//if (cancel == true) printf("User canceled scan");
	//return clean; //returns false if infected and true if clean
	return true;
}

//The engine and options are just passed through to the directory scanner.
//The first parameter is a path the the antibody file
bool LavaScan::QuickScan()
{
	this->num_found = 0; //setting virus counter to 0
	this->start_time = get_time();//getting start time for scan
	//Keeps track of if malware is detected and where
	bool clean = true;
	//Read from the file
	std::vector<std::string> directorylist = ReadAntibody(this->AntibodyFileLocation);
	//For each directory:
	for (int i = 0; i < directorylist.size(); i++)
	{
		//Scan it
		if (clean == true)
		{
			clean = scanDirectory(directorylist[i]);
		} else {
			scanDirectory(directorylist[i]);
		}
	}
	this->finish_time = get_time();//get end time for scan
	//log_scan("Quick",start_time, finish_time, num_found, num_found); //log scan
	isScanDone = true;
	//this->QuarantineContents = this->read_quarantine_contents();
	return true;
}

std::string& replace(std::string& s, const std::string& from, const std::string& to)
{
	if (!from.empty())
		for (size_t pos = 0; (pos = s.find(from, pos)) != std::string::npos; pos += to.size())
			s.replace(pos, from.size(), to);
	return s;
}

bool LavaScan::AdvanceScanNow(std::set<std::string> ss)
{
	// set ss is the set of shit we are gonna scan...simple foreach loop for now
	this-> num_found = 0;//set the virus count to 0
	this-> start_time = get_time(); //get start time for scan
	//int num_found = 0;//set the virus count to 0
	std::string start_time = get_time(); //get start time for scan
	//this->viruses_found = 0;
	//this->viruses_removed = 0;
	for (auto path : ss) {
		//std::cout << "\t" << path << ".\n";
		struct stat s;
		if (stat(path.c_str(), &s) == 0)
		{	
			if (s.st_mode & S_IFDIR)
			{
				//it's a directory, use scandir
				//std::cout << path << " is a directory.";
				scanDirectory(std::string(path+"\\"));
			}
			else if (s.st_mode & S_IFREG)
			{
				//it's a file, use scanfile
				//std::cout << path << " is a file.";
				if (scanFile(path.c_str()) == CL_VIRUS) {
					this->num_found++;
				}
			}
			else
			{
				//something else
				std::cout<<"\n ok wtf is this\n";
			}
		}
		else
		{
			//error
			return false;
		}
	}
	this->finish_time = get_time(); //get end time
	//log_scan("Advanced",start_time, finish_time, num_found, num_found); //logging scan
	isScanDone = true;
	//this->QuarantineContents = this->read_quarantine_contents();
	
	return true;
}

inline bool LavaScan::quarantineIsEmpty() { //checks whether quarantined file is empty
	std::fstream q_file;
	q_file.open("quarantine.lava", std::ios::in | std::ios::out);
	return q_file.peek() == std::ifstream::traits_type::eof();
}

inline void LavaScan::eicarTest(std::string path)
{
	scanFile(path);
	return;
}

inline std::set<std::string> LavaScan::countQuarantineContents() { //prints the contents of the quarantine file
	std::set<std::string> s;
	std::fstream quarantineFile;
	quarantineFile.open("quarantine.lava", std::ios::in | std::ios::out);
	std::string line;
	while (getline(quarantineFile, line)) {
		//std::cout << line << std::endl;
		s.insert(line);
	}
	quarantineFile.close();
	return s;
}

inline struct LavaScan::q_entry {
	std::string old_file_name;
	std::string new_file_name;
	std::string origin_directory;
	std::string virus_name;
	bool operator <(const q_entry& q) const
	{
		return (origin_directory + new_file_name) < (q.origin_directory + q.new_file_name);
	}
};

inline std::set<LavaScan::q_entry> LavaScan::read_quarantine_contents() { //prints the contents of the quarantine file
	std::fstream quarantineFile;
	quarantineFile.open("quarantine.lava", std::ios::in | std::ios::out);
	std::set<struct q_entry> entries;
	std::string line;
	while (getline(quarantineFile, line)) {
		struct q_entry q;
		int found = 0, prev_found = 0, comma_num = 0;
		while (found != std::string::npos) { //extracting quarantined file info from quarantine entry
			prev_found = found;
			found = line.find(",", found + 1, 1);
			int size = found - prev_found;
			switch (comma_num)
			{
			case 0:
				q.old_file_name = line.substr(prev_found, size); //extract scan start time
			case 1:
				q.new_file_name = line.substr(prev_found + 1, size - 1); //exctract scan finish time
			case 2:
				q.origin_directory = line.substr(prev_found + 1, size - 1); //extract number of viruses found
				q.virus_name = line.substr(found + 1); //extract number of viruses removed
			}
			comma_num++;
		}
		entries.insert(q);
	}
	quarantineFile.close();
	return entries;
}

inline void LavaScan::quarantine_file(std::string filepath, std::string virus_name) { //quarantine file given filepath
	std::fstream quarantineFile;
	quarantineFile.open("quarantine.lava", std::ios::app);
	int pos = filepath.find_last_of("/\\"); // finding position of last "\"
	std::string file_name = filepath.substr(pos + 1); //original file name with extension
	std::string quarantine_file_name = file_name.substr(0, file_name.find(".")); //new name of the file in quarantine
	std::string file_directory = filepath.substr(0, pos + 1); //original file directory path
	int repeat_counter = 0;
	std::string extension = ".lava";
	//altering the new file extension so that files don't repeat in the quarantine
	while (quarantine_contains(quarantine_file_name+extension)) { //check to see if a file with that name already exists in the quarantine directory
		extension = "(" + std::to_string(repeat_counter) + ").lava";
		repeat_counter++;
	}
	quarantine_file_name += extension;
	quarantineFile << file_name + "," + quarantine_file_name + "," + file_directory + "," + virus_name << std::endl; //adding it to the quarantine info file (quarantine.lava)
	quarantineFile.close();
	//move_file(filepath, ".\\LAVA_Quarantine\\", ".lava"); //moving file to quarantine folder
	q_entry q; q.origin_directory = file_directory; q.new_file_name = quarantine_file_name; q.old_file_name = file_name; q.virus_name = virus_name;
	this->QuarantineContents.insert(q);
	move_file(filepath, ".\\LAVA_Quarantine\\" + quarantine_file_name); //moving file to quarantine folder
}

inline bool LavaScan::quarantine_contains(std::string file_name) { //function to check if the new file name is already used in the quarantine entry file
	std::ifstream quarantine_file("quarantine.lava", std::ios::out);
	std::string line;
	while(getline(quarantine_file, line)) { //iterate file
		int pos1 = line.find(",");
		int pos2 = line.find(",", pos1 + 1);
		std::string new_name = line.substr(pos1 + 1, pos2-pos1-1); //extract the new file name from quarantine file
		if (new_name == file_name) return true;
	}
	return false;
}

//function to move file with changing the extension
inline void LavaScan::move_file(std::string file_path, std::string new_location_with_new_filename) {
	std::ifstream from(file_path, std::ios::in | std::ios::binary);
	std::ofstream to(new_location_with_new_filename, std::ios::out | std::ios::binary);
	to << from.rdbuf();
	from.close();
	to.close();
	_unlink(file_path.c_str()); //removing the original file
}

inline bool LavaScan::remove_quarantined_files(std::set<std::string> to_remove) { //returns false if errors occurred
	if (to_remove.size() == 0)
		return true;
	bool success = true;
	std::fstream quarantineFile;
	std::fstream temp;
	temp.open("temp.lava", std::fstream::out); //creating a temporary file to store the undeleted files
	quarantineFile.open("quarantine.lava", std::ios::in | std::ios::out); //opening the quarantine file
	temp << "";
	std::string line;
	while (getline(quarantineFile, line)) {
		int found = 0, prev_found = 0, comma_num = 0;
		std::string file_name, file_quarantine_name, file_origin, virus_name;
		while (found != std::string::npos) { //extracting quarantined file info from quarantine entry
			prev_found = found;
			found = line.find(",", found + 1, 1);
			int size = found - prev_found;
			switch (comma_num)
			{
			case 0:
				file_name = line.substr(prev_found, size); //extract file original name
			case 1:
				file_quarantine_name = line.substr(prev_found + 1, size - 1); //exctract file quarantine name
			case 2:
				file_origin = line.substr(prev_found + 1, size - 1); //extract file origin
				virus_name = line.substr(found + 1); //extract virus name
			}
			comma_num++;
		}
		if (to_remove.find(file_name) == to_remove.end()) {
			//moving file back to its original location
			//move_file(".\\LAVA_Quarantine\\" + file_quarantine_name, file_origin, file_name.substr(file_name.find(".")));
			//temp << line << std::endl; //keeping it in quarantine //commenting this out for now. no need to write to file after removing.
			//move_file(".\\LAVA_Quarantine\\" + file_quarantine_name, file_origin + file_name);
			//temp << line << std::endl; //keeping it in quarantine
		}
		else {
			if (_unlink((".\\LAVA_Quarantine\\" + file_quarantine_name).c_str()) != 0)
			{
				//Unable to remove file, keep it in the quarantine folder
				//error message
				//temp << line << std::endl; //again, we dont want to log if we just move back, may be temporary
				success = false;
			}
			else {
				//removal successful
			}
		}
	}
	temp.close();
	quarantineFile.close();
	_unlink("quarantine.lava");
	std::rename("temp.lava", "quarantine.lava");
	return success;
}

inline bool LavaScan::postMoveQuarantine(std::set<q_entry> to_remove) { //after a file has been moved back, remove from qaurantine.lava
	if (to_remove.size() == 0)
		return true;
	std::ofstream ofs;
	ofs.open("quarantine.lava", std::ofstream::out | std::ofstream::trunc);
	ofs.close();
	return true;
}

inline void LavaScan::make_quarantine_directory() {
	struct stat buf;
	stat(".\\LAVA_Quarantine", &buf);
	//check if the quarantine directory exists
	if (!(buf.st_mode & S_IFDIR)) {
		//case if it doesn't exist
		printf("Quarantine directory not found.\n Attempting to make the Quarantine Directory...\n");
		if (int ret=CreateDirectoryA(".\\LAVA_Quarantine", NULL)==0) {
			if (ret == ERROR_ALREADY_EXISTS) {
				//direcotry already exists
				printf("Quarantine Direcotory Ready\n");
			}
			else if(ret== ERROR_PATH_NOT_FOUND){
				//idk how this could happen
			}
			else {
				//some other error
				std::cout << "Error making the quarantine directory, error: " + GetLastError() << std::endl;
			}
		}
		else printf("Quarantine Directory Ready\n");
	}
}

inline bool LavaScan::CompleteScan() {
	this->start_time = get_time();
	bool clean = true;
	//this->viruses_found = 0;
	//this->viruses_removed = 0;
	// for now just scan c$
	//   later get func to get drive letters
	const char* dirs = "C:\\";
	clean = this->scanDirectory(dirs);
	this-> finish_time = get_time();
	//log_scan("Complete",start_time, finish_time, num_found, num_found);
	isScanDone = true;
	//this->QuarantineContents = this->read_quarantine_contents();
	return clean;
}

inline void LavaScan::log_scan() {
	std::fstream log_file;
	log_file.open("scan_log.lava", std::ios::app);
	log_file << this->currentScanGoing + "," + this->start_time + "," + this->finish_time + "," + std::to_string(this->num_found) + "," + std::to_string(this->num_removed) + "\n";
	log_file.close();
	this->reset_QC();//reset qc state after logging every scan
}

inline std::string LavaScan::get_time() {
	time_t rawtime;
	struct tm timeinfo;
	char buffer[40];

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	strftime(buffer, 40, "%c", &timeinfo);
	return (std::string)buffer;
}

inline std::vector<std::vector<std::string>> LavaScan::read_log() {
	std::vector<std::vector<std::string>> ret;

	std::fstream log_file;
	std::stack<std::string> log_order; // using stack to get chronological log order
	log_file.open("scan_log.lava");
	std::string line; //string to store line charaacters
	while (getline(log_file, line)) {
		log_order.push(line); //filling the stack oldest in first, last out
	}
	std::string scan_type, scan_strt, scan_fin, num_found, num_removed; // variable to store the virus info
	while (!log_order.empty()) {
		line = log_order.top(); //get value from log stack
		log_order.pop();//remove value from top
		int found = 0, prev_found = 0, comma_num = 0;
		while (found != std::string::npos) {
			prev_found = found;
			found = line.find(",", found + 1, 1);
			int size = found - prev_found;
			switch (comma_num)
			{
			case 0:
				scan_type = line.substr(prev_found, size); //extract the scan type
			case 1:
				scan_strt = line.substr(prev_found + 1, size - 1); //extract scan start time
			case 2:
				scan_fin = line.substr(prev_found + 1, size - 1); //exctract scan finish time
			case 3:
				num_found = line.substr(prev_found + 1, size - 1); //extract number of viruses found
				num_removed = line.substr(found + 1); //extract number of viruses removed
			}
			comma_num++;
		}
		//values of line are extracted (USE THEM FOR GUI HERE)
		//std::cout << "Type of scan: " + scan_type + ", Start time: " + scan_strt + ", End time: " + scan_fin + ", Number of viruses found: " + num_found + ", Number of viruses removed: " + num_removed + "\n" << std::endl;
		if (scan_type != "") {
			// only add to ret if there is content
			std::vector<std::string> temp; temp.push_back(scan_type); temp.push_back(scan_strt);
			temp.push_back(scan_fin); temp.push_back(num_found); temp.push_back(num_removed);
			ret.push_back(temp);
			temp.clear();
		}
	}
	return ret;
}

//inline bool LavaScan::QuickScan() {
//	// for now just scan docs and downloads 
//	//return this->scanDirectory(dirs, this->engine, this->options);
//	TCHAR my_profile[MAX_PATH];
//	SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, SHGFP_TYPE_CURRENT, my_profile);
//	//std::wcout << "\nscanning " << my_profile << " and \n" << my_profile << "\\..\\Downloads\\\n";
//	char home[MAX_PATH];
//	wcstombs(home, my_profile, MAX_PATH);
//	// convert to string..
//	std::string s_home = std::string(home); std::string docs = s_home + "\\Documents"; std::string dl = s_home + "\\Downloads";
//	scanDirectory(docs, this->engine, this->options);
//	scanDirectory(dl, this->engine, this->options);
//	return true;
//}

inline void LavaScan::UpdatePreviousScans() {
	this->PreviousScans = this->read_log();
}

inline bool LavaScan::moveQuarantineHome(std::set<q_entry> q)
{
	for (auto thing : q) {
		move_file(".\\LAVA_Quarantine\\"+thing.new_file_name, thing.origin_directory+thing.old_file_name); //moving file to quarantine folder
	}
	this->postMoveQuarantine(q);
	return true;
}

inline LavaScan::LavaScan() {
	CurrentScanCount = 0;
	currentScanGoing = "";
	num_found = 0; num_removed = 0;
	isScanDone = false;
	start_time = std::string("");
	finish_time = std::string("");
	if ((ret = cl_init(CL_INIT_DEFAULT)) != CL_SUCCESS) { //Initializing clamav
		printf("Can't initialize libclamav: %s\n", cl_strerror(ret));//returns the error name in case of error
		exit(2);
	}
	else {
		printf("initialization successful\n");
	}

	if (!(engine = cl_engine_new())) { //Creating new engine instance
		printf("Can't create new engine\n");
		exit(1);
	}
	else {
		printf("ClamAV engine initialized\n");
	}

	printf("Inititalizing signature database...\n");
	printf("Default database path: %s\n", cl_retdbdir());

	if ((ret = cl_load(cl_retdbdir(), engine, &sigs, CL_DB_STDOPT)) != CL_SUCCESS) { //Loads the database file from the default db folder
		printf("Can't initialize signature database: %s\n", cl_strerror(ret)); //returns the error name in case of error
		exit(2);
	}
	else {
		printf("Signature database initialization successful\n %u Signatures loaded\n", sigs);
	}

	if ((ret = cl_engine_compile(engine)) != CL_SUCCESS) { //Compiles the ClamAV engine
		printf("cl_engine_compile() error: %s\n", cl_strerror(ret));//returns the error name in case of error
		cl_engine_free(engine);
		exit(1);
	}
	else {
		printf("ClamAV engine ready!\n");
	}

	//printf("\n\n Testing scanFile and scanDirectory functions:\n\n");
	//Testing scanFile function
	
	this->options.general = CL_SCAN_GENERAL_ALLMATCHES;

	//create the quarantine direcory if it doesn't exist
	make_quarantine_directory();

	//Testing Quarantine Functions
	//quarantine_file("C:\\Users\\dylan\\Downloads\\a_virus.txt", "too cool");
	//quarantine_file("C:\\Users\\dylan\\Documents\\a_virus.txt", "trojan");
	//quarantine_file("C:\\Users\\dylan\\Pictures\\a_virus.txt", "trojan");
	std::set<std::string> to_remove;
	//to_remove.insert("a_virus.txt");
	remove_quarantined_files(to_remove);
	//std::set<struct q_entry> entries = read_quarantine_contents();
	//for (struct q_entry q :entries) {
		//std::cout << "File name: " + q.old_file_name + ", new name: " + q.new_file_name + ", origin directory: " + q.origin_directory + ", virus name: " + q.virus_name << std::endl;
	//}

	//std::string AntibodyFileLocation = "C:\\test.LavaAnti";
	////Testing scanDirectory function
	//bool clean = scanDirectory("C:\\Users\\tom\\Documents\\", engine, options, AntibodyFileLocation);
	//if (clean) printf("Directory is not infected");
	//else printf("Directory is not infected");

	//Testing quickScan function
	/*clean = quickScan(AntibodyFileLocation, engine, options);
	if (clean) printf("Directory is not infected");
	else printf("Directory is not infected");*/

	// have a seperate thread load shit into PreviousScans...race condition if many scans on a pc maybe...we will see
	std::thread t1 = std::thread([this] {this->UpdatePreviousScans(); });
	t1.detach();
}

#endif