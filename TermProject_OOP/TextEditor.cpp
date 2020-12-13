#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>

using namespace std;
static int pageIndex = 0;

class TEXT {
public:
	vector<string> lines;
	vector<string> words;
	vector<int> lastWord;
	int index = 0;
};

TEXT split_75Bytes(vector<string> words) {
	vector<string> originWords = words;
	vector<string> lines;
	vector<int> lastWord;
	string tmp;

	for (int i = 0; i < words.size(); i++) {
		tmp += words[i] + " ";

		if (tmp.length() > 75) {
			if (tmp.length() == 76) {
				lines.push_back(tmp);
				lastWord.push_back(i);
				tmp = "";
				continue;
			}

			tmp.erase(tmp.length() - (words[i].length() + 1), tmp.length());
			lines.push_back(tmp);
			lastWord.push_back(i - 1);
			tmp = words[i] + " ";
		}

		else if (i == words.size() - 1 && tmp.length() <= 75) {
			lines.push_back(tmp);
			tmp = "";
		}
	}
	return { lines, originWords, lastWord, 0 };
}

class CMD {
public:
	TEXT text;
	int line;
	int number;
	string word;
	vector<int> lastWord;
	int set = 0;

	CMD(TEXT text) {
		this->text = text;
	}

	virtual TEXT work() { return text; }
};

class CMD_C : public CMD {
public:
	string findWord;
	string changeWord;

	using CMD::CMD;
	CMD_C(string findWord, string changeWord) : CMD(text) {
		this->findWord = findWord;
		this->changeWord = changeWord;
	}

	TEXT work(TEXT text) {
		vector<string> resultWords = text.words;

		for (int i = 0; i < resultWords.size(); i++) {
			if (findWord == text.words[i]) {
				set = 1;
				resultWords[i] = changeWord;
			}
		}
		if (set == 0) {
			throw range_error("txt파일에 " + findWord + "가 없습니다.");
		}
		return split_75Bytes(resultWords);
	}
};

class CMD_D : public CMD {
public:
	CMD_D(TEXT text, int line, int number, vector<int> lastWord) : CMD(text) {
		this->text = text;
		this->line = line;
		this->number = number;
		this->lastWord = lastWord;
	}

	TEXT work() {
		if (line == 1) {
			if (number > lastWord[0] + 1) {
				throw out_of_range("입력된 문자 위치는 해당 문장 길이를 넘어섰습니다.다시 입력하세요.");
			}
			text.words.erase(text.words.begin() + number - 1);
			return split_75Bytes(text.words);
		}

		else {
			if (line > text.lines.size()) {
				throw out_of_range("입력된 문자 위치는 해당 문장 길이를 넘어섰습니다.다시 입력하세요.");
			}
			int index = lastWord[line - 2] + number;
			if (index > lastWord[line - 1] + 1) {
				throw out_of_range("입력된 문자 위치는 해당 문장 길이를 넘어섰습니다.다시 입력하세요.");
			}
			text.words.erase(text.words.begin() + index);
			return split_75Bytes(text.words);
		}
	}

};

class CMD_I : public CMD {
public:
	CMD_I(int line, int number, string word, vector<int> lastWord) : CMD(text) {
		this->line = line;
		this->number = number;
		this->word = word;
		this->lastWord = lastWord;
	}

	TEXT work(TEXT text, string word, int line, int number, vector<int> lastWord) {

		if (line == 1) {
			if (number > lastWord[0] + 1) {
				throw out_of_range("입력값이 최대 number보다 큽니다. 다시 써주세요.");
			}
			text.words.insert(text.words.begin() + number, word);
			return split_75Bytes(text.words);
		}
		else if (line > text.lines.size()) {
			throw out_of_range("입력값이 최대 line보다 큽니다. 다시 써주세요.");
		}

		int index = lastWord[line - 2] + number;
		if (index > lastWord[line - 1]) {
			throw out_of_range("입력값이 최대 number보다 큽니다. 다시 써주세요.");
		}
		text.words.insert(text.words.begin() + index + 1, word);
		return split_75Bytes(text.words);
	}
};

class CMD_N : public CMD {
public:

	using CMD::CMD;

	TEXT work() {
		TEXT nextPage;
		int lineNum = text.lines.size();
		int pageNum = lineNum / 20;

		if (pageNum * 20 <= text.index + 20) {
			set = 1;
			int nextIndex = lineNum - 20;
			nextPage = split_75Bytes(text.words);
			nextPage.index = nextIndex;
		}
		else {
			int nextIndex = text.index + 20;
			nextPage = split_75Bytes(text.words);
			nextPage.index = nextIndex;
			pageIndex = nextIndex;
			cout << pageIndex;
		}
		return nextPage;
	}
};

class CMD_P : public CMD {
public:
	using CMD::CMD;

	virtual TEXT work() {
		TEXT prevPage;
		int lineNum = text.lines.size();
		int pageNum = lineNum / 20;

		if (text.index - 20 <= 0 || text.index < 20) {
			set = 1;
			int prevIndex = 0;
			prevPage = split_75Bytes(text.words);
			prevPage.index = prevIndex;
		}
		else {
			int prevIndex = text.index - 20;
			prevPage = split_75Bytes(text.words);
			prevPage.index = prevIndex;
			pageIndex = prevIndex;
		}
		return prevPage;
	}
};

class CMD_T : public CMD {
public:
	CMD_T() : CMD(text) { }

	TEXT work(TEXT text) {
		ofstream output;
		string path = "C:/Users/laptop/source/repos/TermProject/TermProject/output.txt";
		output.open(path);

		string content;
		if (output.is_open()) {
			for (int i = 0; i < text.lines.size(); i++) {
				content += text.lines[i].c_str(), 75;
			}
		}
		output.write(content.c_str(), content.size() - 1);
		output.close();

		cout << endl;
		cout << "성공적으로 저장되었습니다. 작업을 종료합니다." << endl;
		cout << "저장경로 : " << path << endl;
		exit(0);

		return text;
	}
};

class Text_Editor {
public:
	string input;
	string cmd = "환영합니다. 명령어를 입력해주세요!";

	TEXT text;
	vector<string> StoreAllWords() {
		vector<string> words;
		ifstream open;
		open.open("test.txt");

		if (!open.is_open()) {
			cout << "해당 위치에 파일이 없습니다.";
		}

		open.seekg(0, ios::end);
		int end = (int)open.tellg();
		input.resize(end);
		open.seekg(0, ios::beg);
		open.read(&input[0], end);

		string buffer;
		stringstream stream(input);
		while (stream >> buffer)
			words.push_back(buffer);

		open.close();
		return words;
	}

	TEXT Parse_CMD(string cmd, string UserInput, TEXT text, int index) {
		vector<int> lastWord = text.lastWord;
		vector<string> parameter;

		char main = UserInput[0];
		string str = ""; str = main;

		if (main == 'c' || main == 'd' || main == 'i' || main == 'n' || main == 'p' || main == 's' || main == 't') {
			if (UserInput.size() == 1) {
				if (main == 'n' || main == 'p' || main == 't') {
				}
				else {
					this->cmd = "명령어 " + str + "는 매개변수가 필요합니다.";
					return text;
				}
			}
		}
		else {
			this->cmd = "명령어 " + str + "는 유효한 명령어가 아닙니다.";
			return text;
		}

		if (main == 'c' || main == 'd' || main == 'i' || main == 's') {
			if (UserInput.find('(') == string::npos || UserInput.find(')') == string::npos) {

				this->cmd = "괄호를 제대로 입력해주세요.";
				return text;

				if (UserInput.at(1) != '(') {
					this->cmd = "명령어 옆에는 괄호를 꼭 붙여주세요";
					return text;
				}
			}

			if (UserInput.at(2) == ')') {
				this->cmd = "매개변수가 없습니다.";
				return text;
			}

			if (main != 's') {
				if (UserInput.find(',') == string::npos) {
					this->cmd = "매개변수는 ,로 분리해주세요";
					return text;
				}
			}

			string param = UserInput.substr(2, UserInput.length() - UserInput.find('('));
			param.resize(param.length() - 1);

			string tmp;
			stringstream stream(param);

			while (getline(stream, tmp, ',')) {
				parameter.push_back(tmp);
			}
		}

		if (main == 'c') {
			string findWord = parameter[0];
			string changeWord = parameter[1];

			try {
				CMD_C* cmd_C = new CMD_C(findWord, changeWord);
				text = cmd_C->work(text);
			}
			catch (out_of_range& e) {
				this->cmd = e.what();
				return text;
			}

			this->cmd = "입력하신 " + findWord + "를 " + changeWord + "로 바꿨습니다.";
			return text;
		}

		else if (main == 'd') {
			//int line = stoi(parameter[0]);
			//int number = stoi(parameter[1]);
			int line = atoi(parameter[0].c_str());
			int number = atoi(parameter[1].c_str());

			if (number == 0 || line == 0) {
				this->cmd = "매개변수에 0 혹은 숫자가 아닌 값이 입력되었습니다. 1 이상의 숫자를 입력해주세요.";
				return text;
			}

			if (line <= pageIndex || line > pageIndex + 20) {
				this->cmd = "현재 페이지에 없는 줄입니다.";
				return text;
			}

			try {
				CMD_D* cmd_D = new CMD_D(text, line, number, lastWord);
				text = cmd_D->work();
				this->cmd = to_string(line) + "번째 줄의 " + to_string(number) + "번째 단어가 삭제되었습니다.";
			}
			catch (out_of_range& e) {
				this->cmd = e.what();
			}
			return text;
		}

		else if (main == 'i') {
			int line = atoi(parameter[0].c_str());
			int number = atoi(parameter[1].c_str());
			string word = parameter[2];

			if (number == 0 || line == 0) {
				this->cmd = "앞의 두 인자는 숫자여야 하고, 0이면 안 됩니다.";
				return text;
			}

			if (word.size() >= 75) {
				this->cmd = "삽입할 문자열은 75바이트 이하여야 합니다.";
				return text;
			}

			try {
				CMD_I* cmd_I = new CMD_I(line, number, word, lastWord);
				text = cmd_I->work(text, word, line, number, lastWord);
			}
			catch (out_of_range& e) {
				this->cmd = e.what();
				return text;
			}
			this->cmd = word + " 가 성공적으로 삽입되었습니다!";
			return text;
		}

		else if (main == 'n') {
			CMD_N* cmd_N = new CMD_N(text);
			TEXT result = cmd_N->work();

			if (cmd_N->set != 1) { this->cmd = "다음 페이지입니다."; }
			else { this->cmd = "마지막 페이지입니다."; }
			return result;
		}

		else if (main == 'p') {
			CMD_P* cmd_P = new CMD_P(text);
			TEXT result = cmd_P->work();

			if (cmd_P->set != 1) { this->cmd = "이전 페이지입니다."; }
			else { this->cmd = "첫 페이지입니다."; }
			return result;
		}

		else if (main == 't') {
			CMD_T* cmd_T = new CMD_T();
			this->cmd = "저장 후 종료합니다.";
			return cmd_T->work(text);
		}
	}

	string Input_CMD() {
		for (int i = 0; i < 20; i++) {
			if (text.index == 0 && i < 9) {
				cout << " ";
			}
			cout << this->text.index + i + 1 << "| " << this->text.lines[this->text.index + i] << endl;
		}
		cout << "--------------------------------------------------------------------------------" << endl;
		cout << "n:다음페이지, p:이전페이지, i:삽입. d:삭제, c:변경, s:찾기, t:저장후종료 " << endl;
		cout << "--------------------------------------------------------------------------------" << endl;
		cout << "( 콘솔메시지 ) : " << this->cmd << endl;
		cout << "--------------------------------------------------------------------------------" << endl;
		cout << "입력 : ";

		string UserInput;
		cin >> UserInput;

		this->text = Parse_CMD(cmd, UserInput, text, text.index);
		cout << "--------------------------------------------------------------------------------" << endl;

		return UserInput;
	}

	void Run() {
		vector<string> words = StoreAllWords();
		this->text = split_75Bytes(words);

		string input;
		while (true) {
			input = Input_CMD();
		}
	}
};

int main() {
	Text_Editor* textEditor = new Text_Editor();
	textEditor->Run();
	return 0;
}