#include<stdio.h>
#include<iostream>
#include<list>
#include<string>
#include<fstream> // 파일 입출력을 위한 fstream
#include<algorithm>


#define SIZE 100
#define CHART_SIZE 10
using namespace std;

typedef struct grammar {
	string leftSide;
	list<string> rightSide;
}Grammar;

typedef struct inputList {
	list<string> sentence;
}InputList;

typedef struct chart {
	list<string> element;
}Chart;

//문자열 처리 함수
void split(const string& s, string delim, list<string>& v) {
	auto i = 0;
	auto pos = s.find(delim);
	
	//rightside에 하나만 있을 경우 그대로 반환함
	if (pos == string::npos)
		v.push_back(s);

	while (pos != string::npos) {
		v.push_back(s.substr(i, pos - i));
		i = ++pos;
		pos = s.find(delim, pos);

		if (pos == string::npos)
			v.push_back(s.substr(i, s.length()));
	}
}

//결과값 확인용 함수
void printData(Grammar &data) {
	list<string>::iterator iter;
	cout << data.leftSide + " ->";
	for (iter = data.rightSide.begin(); iter != data.rightSide.end(); iter++) {
		cout << ' ' << *iter << ' ';
		//printf("dd");
	}
	cout << '\n';
		
}


int main(void) {
	
	
	char inputString[SIZE] = "";
	ifstream ingmrFile("grammar.txt");
	ofstream outputFile("output.txt", ios::out|ios::app);

	list<Grammar> gmrList;
	list<Grammar>::iterator itForGmr;
	Grammar temp;
	string delimiter = " -> ";
	size_t pos = 0;
	string token;
	int i = 0;

	Chart chart[CHART_SIZE][CHART_SIZE];

	

	/************************************
	1. grammar 읽어오기
	*************************************/

	while (!ingmrFile.eof()) {
		ingmrFile.getline(inputString, 100);
		string s = inputString;
		
		//공백 예외처리
		if (s == "")
			continue;

		pos = s.find(delimiter);
		if(s.substr(0,pos)!="")
			temp.leftSide = s.substr(0, pos);
		//printf("%d", pos);
		if (pos + delimiter.length() < s.length())
			split(s.substr(pos + delimiter.length(), s.length()), " ", temp.rightSide);
		else
			break;

		gmrList.push_back(temp);
		//list를 비워준다.
		temp.rightSide.clear();
	}
	
	//파일 내용 입력 확인
	for_each(gmrList.begin(), gmrList.end(), printData);

	/************************************
	2. input 문장 읽어들이기
	*************************************/
	list<InputList> input;
	InputList tempForInput;
	list<InputList>::iterator it;
	string delimiterForInput = " ";
	ifstream inputFile("input.txt");
	
	while (!inputFile.eof()) {
		inputFile.getline(inputString, 100);
		string s = inputString;

		//공백 예외처리
		if (s == "")
			continue;
		
		split(s, " ", tempForInput.sentence);
		input.push_back(tempForInput);

		tempForInput.sentence.clear();
		
	}
	
	//파일내용확인
	//for (it = input.begin(); it != input.end(); ++it) {
	//	list<string>::iterator iter;
	//	for (iter = it->input.begin(); iter != it->input.end(); ++iter) {
	//		cout << *iter << ' ';
	//	}
	//}

	/************************************
	3. 알고리즘적용
	*************************************/
	list<InputList>::iterator iterForCYK;
	list<string>::iterator iterForPrint;
	list<string>::iterator iterForPrintL;
	list<string>::iterator iterForPrintR;

	for (iterForCYK = input.begin(); iterForCYK != input.end(); ++iterForCYK) { // 문장 단위 loop

		int finalResultIndex = 0;
		//차트 초기화
		for (int i = 0; i < CHART_SIZE; i++) {
			for (int j = 0; j < CHART_SIZE; j++) {
				chart[i][j].element.clear();
			}
		}

		outputFile << "Grammars" << endl << endl;
		list<string>::iterator iterForPhr;
		list<string>::iterator iterForRightSide;
		for (iterForPhr = iterForCYK->sentence.begin(); iterForPhr != iterForCYK->sentence.end(); ++iterForPhr) { // 구 단위 loop
			//1. 초기값 적용
			for (itForGmr = gmrList.begin(); itForGmr != gmrList.end(); ++itForGmr) { //문법 탐색 loop
				for (iterForRightSide = itForGmr->rightSide.begin(); iterForRightSide != itForGmr->rightSide.end(); ++iterForRightSide){ //문법 rightSide loop
					string rSide = *iterForRightSide;
					string phrase = *iterForPhr;
					if (rSide == phrase){
						int i = distance(iterForCYK->sentence.begin(),iterForPhr); //iterator의 index를 구한다.
						outputFile << itForGmr->leftSide << "->" << rSide << endl;
						cout << itForGmr->leftSide << "->" << rSide << endl; //해당 문법 출력
						chart[i][i+1].element.push_back(itForGmr->leftSide + " ( " + rSide + " )"); //가능성 있는 모든 해당 문법을 chart에 넣는다.
						if (i >= finalResultIndex)
							finalResultIndex = i+1;
					}
				}
				//cout << "phrase passed";

			}
		}
		for (iterForPhr = iterForCYK->sentence.begin(); iterForPhr != iterForCYK->sentence.end(); ++iterForPhr) { // 구 단위 loop
			for (itForGmr = gmrList.begin(); itForGmr != gmrList.end(); ++itForGmr) { //문법 탐색 loop
				if(itForGmr->rightSide.size() == 1){
					string rSide = *(itForGmr->rightSide.begin());
					int i = distance(iterForCYK->sentence.begin(), iterForPhr); //iterator의 index를 구한다.
					for (iterForPrintL = chart[i][i + 1].element.begin(); iterForPrintL != chart[i][i + 1].element.end(); iterForPrintL++) {
						string sL = *iterForPrintL;
						
						size_t foundL = sL.find(" ");

						string sp_sL = sL.substr(0, foundL); //leftside

						if (rSide == sp_sL) {
							outputFile << itForGmr->leftSide << "->" << sp_sL << endl;
							cout << itForGmr->leftSide << "->" << sp_sL << endl; //해당 문법 출력
							chart[i][i + 1].element.push_back(itForGmr->leftSide + " ( " + sL + " ) ");
						}

					}
				}
			}
		}

		//chart [0][1]의 값 " " chart[1][2]의 값이 gmrLIst와 일치한다면
		//chart[0][2]에 해당 gmr의 leftside ( chart[0][1] chart[1][2] ) 를 넣는다.
		for (int x = 0; x < CHART_SIZE;x++){
		for (iterForPhr = iterForCYK->sentence.begin(); iterForPhr != iterForCYK->sentence.end(); ++iterForPhr) { // 구 단위 loop
			int i = distance(iterForCYK->sentence.begin(), iterForPhr); //iterator의 index를 구한다.
			for (int j = 1; j < CHART_SIZE; j++) {
				for (itForGmr = gmrList.begin(); itForGmr != gmrList.end(); ++itForGmr) { //문법 탐색 loop
					//for (iterForRightSide = itForGmr->rightSide.begin(); iterForRightSide != itForGmr->rightSide.end(); ++iterForRightSide) { //문법 rightSide loop
					if (itForGmr->rightSide.size() > 1) {
						string element1 = *itForGmr->rightSide.begin();
						auto element2 = itForGmr->rightSide.begin();
						advance(element2, 1); 
						string element3 = *element2;
						for (int k = i; k<j; k++) {
							for (iterForPrintL = chart[i][k].element.begin(); iterForPrintL != chart[i][k].element.end(); iterForPrintL++) {
								for (iterForPrintR = chart[k][j].element.begin(); iterForPrintR != chart[k][j].element.end(); ++iterForPrintR) {
									string sL = *iterForPrintL;
									string sR = *iterForPrintR;
									size_t foundL = sL.find(" ");
									size_t foundR = sR.find(" ");
									string sp_sL = sL.substr(0, foundL); //leftside
									string sp_sR = sR.substr(0, foundR); //leftside
									if (element1 == sp_sL && element3 == sp_sR) {
										if(chart[i][j].element.empty()){
											outputFile << itForGmr->leftSide << "->" << sp_sL << " " << sp_sR << endl;
											cout <<"chart: "<< i<<" " << j <<" : "<< itForGmr->leftSide << "->" << sp_sL <<" "<< sp_sR << endl; //해당 문법 출력	
										}
										//중복을 제거하고 삽입
										chart[i][j].element.remove(itForGmr->leftSide + " ( " + sL + " " + sR + " ) ");
										chart[i][j].element.push_front(itForGmr->leftSide + " ( " + sL + " " + sR + " ) ");
									}

								}
							}
						}
						

					}
				}
			}
			}
		
		}

		outputFile << endl << "Parse Tree" << endl << endl;
		for (iterForPrint = chart[0][finalResultIndex].element.begin(); iterForPrint != chart[0][finalResultIndex].element.end(); iterForPrint++)
		{
			string s = *iterForPrint;
			size_t found = s.find(" ");
			string sp_sL = s.substr(0, found); //leftside
			if(sp_sL =="S") //완성된 형태의 parse tree 출력
				outputFile << s << endl<<endl;
		}
		

		//차트 내용 프린트
		//for (int i = 0; i < CHART_SIZE; i++) {
		//	for (int j = 0; j < CHART_SIZE; j++) {
		//		for (iterForPrint = chart[i][j].element.begin(); iterForPrint != chart[i][j].element.end(); iterForPrint++)
		//		{

		//			cout << "index: " << i << ' ' << j << " content: " << *iterForPrint << endl;
		//		}

		//	}
		//}

	}

	inputFile.close();
	outputFile.close();

	system("pause");
	return 0;
}