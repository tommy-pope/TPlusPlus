#include "parser.h"

using namespace std;

string buff[1000];
int buffSize = 0;

int tokenArr[1000] = { 0 };
int arrPointer = 0;

string strLitContainer[100];
int strLitCounter = 0;

int numLitContainer[100];
int numLitCounter = 0;

string svarContainer[100];
int svarCounter = 0;
string svarValues[100];
int svarValuesCounter = 0;

string nvarContainer[100];
int nvarCounter = 0;
int nvarValues[100];
int nvarValuesCounter = 0;

string pvarContainer[100];
int pvarCounter = 0;
int pvarValues[100];
int pvarValuesCounter = 0;

int readFileInBuf(fstream& file);
int parseBuff();
int parseWord(string word);
int translate();

string subString(string word, int left, int right);
bool isBreak(char c);
int determineWord(string c, int length);


int main(int argc, char** argv)
{
	// handling argument errors
	if (argc < 2) {
		cout << "Error: No .t file provided to compile." << endl;
		return -1;
	}
	else if (argc > 2) {
		cout << "Error: Too many arguments provided." << endl;
		return -1;
	}

	// begin attempting to open file
	fstream file;

	file.open(argv[1], fstream::in);

	// file failed to open
	if (!file.is_open()) {
		cout << "Error: Failed to find file." << endl;
		return -1;
	}

	readFileInBuf(file);

	if (parseBuff() == -1) {
		cout << "Error: Invalid Syntax." << endl;
		return -1;
	};

	file.close();

	cout << endl << "T++ program execution finished." << endl;

	return 0;
}

int readFileInBuf(fstream& file) {
	string tmp;

	while (getline(file, tmp)) {
		tmp.resize(tmp.length() + 1);
		tmp[tmp.length()-1] = '\n';

		buff[buffSize] = tmp;
		buffSize++;
	}

	return 0;
}

int parseBuff() {
	for (int i = 0; i < buffSize; i++) {
		if (parseWord(buff[i]) == -1) {
			return -1;
		};
	}

	cout << "Parsed file successfully. Beginning syntax analysis." << endl;

	if (translate() == -1) {
		return -1;
	};

	return 0;
}

int parseWord(string word) {
	int left = 0;
	int right = 0;
	int len = (int)word.length();

	while (right <= len && left <= right) {
		if (isBreak(word[right])) {
			if (word[left] == '"') {
				int strLen = 0;

				while (word[right] != '"') {
					if (strLen > 80) {
						return -1;
					}

					right++;
					strLen++;
				}

				right++;
			}

			string sub = subString(word, left, right);

			int token = 0;
			int length = right - left;
			token = determineWord(sub, length);

			if (token == -1) {
				cout << "Error: Bad Token." << endl;
				return -1;
			}

			tokenArr[arrPointer] = token;
			arrPointer++;

			right = right + 1;
			left = right;
		}



		right++;
	}

	return 0;
}

int translate() {
	int token = 0;
	stack<int> loopStack;

	cout << "T++ program starting..." << endl << endl;

	for (int i = 0; i < arrPointer; i++) {
		token = tokenArr[i];

		if (token == PRINT) {
			string output;

			if (tokenArr[i + 1] == STRING_LIT) {
				int sCount = 0;

				for (int s = 0; s < i; s++) {
					if (tokenArr[s] == STRING_LIT) {
						sCount++;
					}
				}

				output = strLitContainer[sCount];
			}
			else if (tokenArr[i + 1] == STRING_VAR || tokenArr[i + 1] == NUMBER_VAR) {
				if (tokenArr[i + 2] != VAR_NAME) {
					cout << "Expected VARNAME." << endl;
					return -1;
				}

				int typeVar = tokenArr[i + 1];
				int sCount = 0;

				for (int s = 0; s < i; s++) {
					if (tokenArr[s] == typeVar) {
						sCount++;
					}
				}

				int pos = -1;

				if (typeVar == STRING_VAR) {
					for (int b = 0; b < svarCounter; b++) {
						if (svarContainer[b] == svarContainer[sCount]) {
							pos = b;
							break;
						}
					}

					if (pos == -1) {
						cout << "Incorrect typing for VARNAME" << endl;
						return -1;
					}

					output = svarValues[pos];
				}
				else if (typeVar == NUMBER_VAR) {
					int count = 0;

					for (int b = 0; b < nvarCounter; b++) {
						bool dup = false;

						for (int f = 0; f < b; f++) {
							if (nvarContainer[f] == nvarContainer[b]) {
								dup = true;
								break;
							}
						}

						if (nvarContainer[b] == nvarContainer[sCount]) {
							pos = count;
							break;
						}

						if (!dup) {
							count++;
						}

					}

					if (pos == -1) {
						cout << "Incorrect typing for VARNAME" << endl;
						return -1;
					}

					output = to_string(nvarValues[pos]);
				}

			}
			else {
				cout << "Expected either a STRING LITERAL or a VARNAME." << endl;
				return -1;
			}
			
			cout << output << endl;

			i += 2;
		}
		else if (token == STRING_VAR) {
			if (tokenArr[i + 1] != VAR_NAME) {
				cout << "Missing VARNAME." << endl;
				return -1;
			}

			if (tokenArr[i + 2] != ASSIGN_OP) {
				cout << "Missing ASSIGN OPERATOR." << endl;
				return -1;
			}

			if (tokenArr[i + 3] != STRING_LIT) {
				cout << "Missing STRING LITERAL." << endl;
				return -1;
			}

			int sCount = 0;

			for (int s = 0; s < i; s++) {
				if (tokenArr[s] == STRING_LIT) {
					sCount++;
				}
			}

			svarValues[svarValuesCounter] = strLitContainer[sCount];
			svarValuesCounter++;
		}
		else if (token == NUMBER_VAR) {
			if (tokenArr[i + 1] != VAR_NAME) {
				cout << "Missing VARNAME." << endl;
				return -1;
			}

			if (tokenArr[i + 2] != ASSIGN_OP) {
				cout << "Missing ASSIGN OPERATOR." << endl;
				return -1;
			}
			
			if (tokenArr[i + 3] != NUMBER_LIT && tokenArr[i + 3] != START_EXP) {
				cout << "Missing NUMBER LITERAL OR EXPRESSION" << endl;
				return -1;
			}

			if (tokenArr[i + 3] == START_EXP) {

				int endEXP = i + 4;
				int secondBreak = 0;

				if (tokenArr[endEXP] != NUMBER_VAR && tokenArr[endEXP] != NUMBER_LIT) {
					cout << "Missing NUMBERVAR/NUMBERLIT in EXPRESSION." << endl;
					return -1;
				}

				// error message handling

				if (tokenArr[endEXP] == NUMBER_VAR) {
					secondBreak = endEXP + 3;

					if (tokenArr[endEXP + 1] != VAR_NAME) {
						cout << "Missing VARNAME in EXPRESSION." << endl;
						return -1;
					}

					if (tokenArr[endEXP + 2] != ADD_OP && tokenArr[endEXP + 2] != SUB_OP && tokenArr[endEXP + 2] != MULTI_OP && tokenArr[endEXP + 2] != DIVIDE_OP) {
						cout << "Missing OPERATOR." << endl;
					}

					if (tokenArr[endEXP + 3] != NUMBER_VAR && tokenArr[endEXP + 3] != NUMBER_LIT) {
						cout << "Missing NUMBERVAR/NUMBERLIT in EXPRESSION." << endl;
						return -1;
					}

					if (tokenArr[endEXP + 3] == NUMBER_VAR) {

						if (tokenArr[endEXP + 4] != VAR_NAME) {
							cout << "Missing VARNAME in EXPRESSION." << endl;
							return -1;
						}

						if (tokenArr[endEXP + 5] != END_EXP) {
							cout << "Missing END OF EXPRESSION." << endl;
							return -1;
						}
					}
					else {
						if (tokenArr[endEXP + 4] != END_EXP) {
							cout << "Missing END OF EXPRESSION." << endl;
							return -1;
						}
 					}
				}
				else {
					secondBreak = endEXP + 2;
					if (tokenArr[endEXP + 1] != ADD_OP && tokenArr[endEXP + 1] != SUB_OP && tokenArr[endEXP + 1] != MULTI_OP && tokenArr[endEXP + 1] != DIVIDE_OP) {
						cout << "Missing OPERATOR." << endl;
					}

					if (tokenArr[endEXP + 2] != NUMBER_VAR && tokenArr[endEXP + 2] != NUMBER_LIT) {
						cout << "Missing NUMBERVAR/NUMBERLIT in EXPRESSION." << endl;
						return -1;
					}

					if (tokenArr[endEXP + 2] == NUMBER_VAR) {
						if (tokenArr[endEXP + 3] != VAR_NAME) {
							cout << "Missing VARNAME in EXPRESSION." << endl;
							return -1;
						}

						if (tokenArr[endEXP + 4] != END_EXP) {
							cout << "Missing END OF EXPRESSION." << endl;
							return -1;
						}
					}
					else {
						if (tokenArr[endEXP + 3] != END_EXP) {
							cout << "Missing END OF EXPRESSION." << endl;
						}
					}
				}

				// finding position of values
				
				int sCount = 0, tCount = 0, aCount = 0;
				int pos = -1, pos2 = -1, pos3 = 0;

				if (tokenArr[endEXP] == NUMBER_VAR) {
					for (int s = 0; s < endEXP; s++) {
						if (tokenArr[s] == NUMBER_VAR) {
							sCount++;
						}
					}

					for (int b = 0; b < nvarCounter; b++) {
						if (nvarContainer[b] == nvarContainer[sCount]) {
							pos = b;
							break;
						}
					}
				}
				else {
					for (int s = 0; s < endEXP; s++) {
						if (tokenArr[s] == NUMBER_LIT) {
							sCount++;
						}
					}
				}

				if (tokenArr[secondBreak] == NUMBER_VAR) {
					for (int t = 0; t < secondBreak; t++) {
						if (tokenArr[t] == NUMBER_VAR) {
							tCount++;
						}
					}

					for (int b = 0; b < nvarCounter; b++) {
						if (nvarContainer[b] == nvarContainer[tCount]) {
							pos2 = b;
							break;
						}
					}
				}
				else {
					for (int s = 0; s < secondBreak; s++) {
						if (tokenArr[s] == NUMBER_LIT) {
							tCount++;
						}
					}
				}

				for (int t = 0; t < i + 1; t++) {
					if (tokenArr[t] == NUMBER_VAR) {
						aCount++;
					}
				}

				// finds position of value to be set
				int count = 0;
				for (int b = 0; b < nvarCounter; b++) {
					bool dup = false;

					for (int f = 0; f <= b; f++) {
						if (nvarContainer[f] == nvarContainer[b]) {
							dup = true;
							break;
						}
					}

					if (nvarContainer[b] == nvarContainer[aCount]) {
						pos3 = count;
						break;
					}

					if (!dup) {
						count++;
					}
				}

				// set values to do math with
				int newVal = 0, fVal = 0, sVal = 0;

				if (pos == -1) {
					fVal = numLitContainer[sCount];
				}
				else {
					fVal = nvarValues[pos];
				}

				if (pos2 == -1) {
					sVal = numLitContainer[tCount];
				}
				else {
					sVal = nvarValues[pos2];
				}

				// determine operation
				if (tokenArr[endEXP] == NUMBER_VAR) {
					if (tokenArr[endEXP + 2] == ADD_OP) {
						newVal = fVal + sVal;
					}
					else if (tokenArr[endEXP + 2] == SUB_OP) {
						newVal = fVal - sVal;
					}
					else if (tokenArr[endEXP + 2] == MULTI_OP) {
						newVal = fVal * sVal;
					}
					else if (tokenArr[endEXP + 2] == DIVIDE_OP) {
						newVal = fVal / sVal;
					}
				}
				else {
					if (tokenArr[endEXP + 1] == ADD_OP) {
						cout << sVal << endl;
						newVal = fVal + sVal;
					}
					else if (tokenArr[endEXP + 1] == SUB_OP) {
						newVal = fVal - sVal;
					}
					else if (tokenArr[endEXP + 1] == MULTI_OP) {
						newVal = fVal * sVal;
					}
					else if (tokenArr[endEXP + 1] == DIVIDE_OP) {
						newVal = fVal / sVal;
					}
				}

				// set value of var
				if (pos < nvarValuesCounter) {
					nvarValues[pos3] = newVal;
				}
				else {
					nvarValues[nvarValuesCounter] = newVal;
					nvarValuesCounter++;
				}

				// determines how much to move forward in tokenarr
				if (tokenArr[endEXP] == NUMBER_VAR) {
					if (tokenArr[endEXP + 3] == NUMBER_VAR) {
						i += 9;
					}
					else {
						i += 8;
					}
				}
				else {
					if (tokenArr[endEXP + 2] == NUMBER_VAR) {
						i += 8;
					}
					else {
						i += 7;
					}
				}
			}
			else {
				int nCount = 0;
				int vCount = 0;
				int pos = -1;

				for (int s = 0; s < i; s++) {
					if (tokenArr[s] == NUMBER_LIT) {
						nCount++;
					}
				}

				for (int s = 0; s < i; s++) {
					if (tokenArr[s] == NUMBER_VAR) {
						vCount++;
					}
				}

				int count = 0;
				for (int b = 0; b < nvarCounter; b++) {
					bool dup = false;

					for (int f = 0; f < b; f++) {
						if (nvarContainer[f] == nvarContainer[b]) {
							dup = true;
							break;
						}
					}

					if (nvarContainer[b] == nvarContainer[vCount]) {
						pos = count;
						break;
					}

					if (!dup) {
						count++;
					}

				}

				if (pos < nvarValuesCounter) {
					nvarValues[pos] = numLitContainer[nCount];
				}
				else {
					nvarValues[nvarValuesCounter] = numLitContainer[nCount];
					nvarValuesCounter++;
				}
			}
		}
		else if (token == POINTER_VAR) {

			if (tokenArr[i + 1] != VAR_NAME) {
				cout << "Missing VARNAME." << endl;
				return -1;
			}

			if (tokenArr[i + 2] != ASSIGN_OP && tokenArr[i + 2] != ADD_OP) {
				cout << "Missing ASSIGN or ADD OPERATOR." << endl;
				return -1;
			} 

			if (tokenArr[i + 2] == ASSIGN_OP) {
				if (tokenArr[i + 3] != NUMBER_VAR) {
					cout << "Missing POINTER TYPE" << endl;
					return -1;
				}

				if (tokenArr[i + 4] != NUMBER_LIT) {
					cout << "Missing POINTER SIZE" << endl;
				}
			
				int lCount = 0;

				for (int x = 0; x < i + 4; x++) {
					if (tokenArr[x] == NUMBER_LIT) {
						lCount++;
					}
				}

				lCount = numLitContainer[lCount];

				for (int x = -1; x < lCount; x++) {

					if (x < 0) {
						nvarValues[nvarValuesCounter] = lCount;
						nvarValuesCounter++;
						continue;
					}

					nvarValues[nvarValuesCounter] = -1;
					nvarValuesCounter++;
				}

				pvarValues[pvarValuesCounter] = nvarValuesCounter - lCount - 1;
				pvarValuesCounter++;

				for (int c = 0; c < nvarValuesCounter; c++) {
					cout << nvarValues[c] << endl;
				}

				cout << "Pointer: " << pvarValues[0] << endl;

				i += 4;
			}
			else {
				if (tokenArr[i + 3] != ASSIGN_OP) {
					return -1;
				}

				



			}
		}
		else if (token == CONDITIONAL) {
			if (tokenArr[i + 1] != NUMBER_VAR) {
				cout << "Expected NUMBERVAR." << endl;
				return -1;
			}

			if (tokenArr[i + 2] != VAR_NAME) {
				cout << "Expected VARNAME." << endl;
			}

			if (tokenArr[i + 3] != LESSTHAN_OP && tokenArr[i + 3] != GREATERTHAN_OP && tokenArr[i + 3] != EQUALS_OP) {
				cout << "Expected OPERATOR." << endl;
			}

			if (tokenArr[i + 4] != NUMBER_VAR) {
				cout << "Expected NUMBERVAR." << endl;
				return -1;
			}

			if (tokenArr[i + 5] != VAR_NAME) {
				cout << "Expected VARNAME." << endl;
			}

			int sCount = 0, tCount = 0;
			int pos = 0, pos2 = 0;

			int endEXP = i + 1;

			for (int s = 0; s < endEXP; s++) {
				if (tokenArr[s] == NUMBER_VAR) {
					sCount++;
				}
			}

			for (int t = 0; t < endEXP + 3; t++) {
				if (tokenArr[t] == NUMBER_VAR) {
					tCount++;
				}
			}

			for (int b = 0; b < nvarCounter; b++) {

				if (nvarContainer[b] == nvarContainer[sCount]) {
					pos = b;
					break;
				}
			}

			for (int b = 0; b < nvarCounter; b++) {
				if (nvarContainer[b] == nvarContainer[tCount]) {
					pos2 = b;
					break;
				}
			}

			if (tokenArr[i + 3] == LESSTHAN_OP) {
				if (nvarValues[pos] < nvarValues[pos2]) {
					i += 5;
				}
				else {
					while (tokenArr[i] != END_FUNC) {
						i++;
					}
				}
			}
			else if (tokenArr[i + 3] == GREATERTHAN_OP) {
				if (nvarValues[pos] > nvarValues[pos2]) {
					i += 5;
				}
				else {
					while (tokenArr[i] != END_FUNC) {
						i++;
					}
				}
			}
			else if (tokenArr[i + 3] == EQUALS_OP) {
				if (nvarValues[pos] == nvarValues[pos2]) {
					i += 5;
				}
				else {
					while (tokenArr[i] != END_FUNC) {
						i++;
					}
				}
			}
		}
		else if (token == START_LOOP) {
			loopStack.push(i);
		}
		else if (token == END_LOOP) {

			if (!loopStack.empty()) {
				i = loopStack.top()-1;
				loopStack.pop();
			}
		}
		else if (token == LEAVE_LOOP) {
			while (!loopStack.empty()) {
				loopStack.pop();
			}
		}

	}

	return 0;
}

string subString(string ostr, int left, int right) {

	string nStr;
	nStr.resize(right - left);

	for (int s = left; s < right; s++) {
		nStr[s - left] = ostr[s];
		nStr[right - left + 1] = '\0';
	}

	return nStr;
}

bool isBreak(char c) {

	if (c == ' ' || c == '"' || c == '(' || c == '\n' || c == '[' || c == '/') {
		return true;
	}

	return false;
}

int determineWord(string c, int length) {
	if (c.compare("print") == 0) {
		return PRINT;
	}
	else if (c.compare("svar") == 0) {
		return STRING_VAR;
	}
	else if (c.compare("nvar") == 0) {
		return NUMBER_VAR;
	}
	else if (c.compare("pvar") == 0) {
		return POINTER_VAR;
	}
	else if (c.compare("~") == 0) {
		return ASSIGN_OP;
	}
	else if (c.compare("+") == 0) {
		return ADD_OP;
	}
	else if (c.compare("-") == 0) {
		return SUB_OP;
	}
	else if (c.compare("*") == 0) {
		return MULTI_OP;
	}
	else if (c.compare("/") == 0) {
		return DIVIDE_OP;
	}
	else if (c.compare("<") == 0) {
		return LESSTHAN_OP;
	}
	else if (c.compare(">") == 0) {
		return GREATERTHAN_OP;
	}
	else if (c.compare("=") == 0) {
		return EQUALS_OP;
	}
	else if (c[0] == '.' && length > 1) {

		if (tokenArr[arrPointer - 1] == NUMBER_VAR) {
			nvarContainer[nvarCounter] = c;
			nvarCounter++;
		}
		else if (tokenArr[arrPointer - 1] == STRING_VAR) {
			svarContainer[svarCounter] = c;
			svarCounter++;
		}
		else if (tokenArr[arrPointer - 1] == POINTER_VAR) {
			pvarContainer[pvarCounter] = c;
			pvarCounter++;
		}

		return VAR_NAME;
	}
	else if (c[0] == '"' && c[length - 1] == '"') {
		string nc;
		nc.resize(length - 2);

		for (int j = 1; j < length - 1; j++) {
			nc[j - 1] = c[j];
		}

		strLitContainer[strLitCounter] = nc;
		strLitCounter++;
		return STRING_LIT;
	}
	else if (c[0] == '(' && c[length - 1] == ')') {
		string nc;
		nc.resize(length - 2);

		for (int j = 1; j < length - 1; j++) {
			nc[j - 1] = c[j];
		}

		int num = stoi(nc);

		numLitContainer[numLitCounter] = num;
		numLitCounter++;
		return NUMBER_LIT;
	}
	else if (c[0] == '[') {
		return START_EXP;
	}
	else if (c[0] == ']') {
		return END_EXP;
	}
	else if (c[0] == '?') {
		return CONDITIONAL;
	}
	else if (c[0] == '/') {
		return START_FUNC;
	}
	else if (c[0] == '\\') {
		return END_FUNC;
	}
	else if (c.compare("sloop") == 0) {
		return START_LOOP;
	}
	else if (c.compare("eloop") == 0) {
		return END_LOOP;
	}
	else if (c.compare("leave") == 0) {
		return LEAVE_LOOP;
	}
	else if (c == "") {
		return BLANK;
	}
	else {
		return -1;
	}
}
