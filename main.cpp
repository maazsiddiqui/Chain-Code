#include <iostream>
#include <fstream>
using namespace std;

class Image{

	public: 
		int numRows;
	    int numCols;
	    int minVal; 
	    int maxVal; 

		int **zeroFramedAry;  
		
		Image(int row, int col, int min, int max) {
			
			numRows = row;
			numCols = col;
			minVal = min;
			maxVal = max;
	
			zeroFramedAry = new int*[row+2];
			for(int i = 0; i < row+2; i++) zeroFramedAry[i] = new int[col+2];	
		}

		//to frame the input 
		void zeroFramed() {
			for(int i = 0; i < numRows+2; i++){
				zeroFramedAry[i][0] = 0;
				zeroFramedAry[i][numCols+1] = 0;
			}
			for(int j = 0; j < numCols+2; j++){
				zeroFramedAry[0][j] = 0;
				zeroFramedAry[numRows+1][j] = 0;
			}		
		} 
		
		void loadImage(ifstream &fileCC) {
			int pixel_val = 0;
			int i = 1, j = 1;
			while(fileCC >> pixel_val) {	
				
				if(j > numCols) {
					i++;
					j = 1;
				}
				zeroFramedAry[i][j] = pixel_val;
				j++;	
			}	
		}
};

class CCProperty {
	public: 
	int maxCC;
	
	struct Property {
		
		int label;
		int noOfPixels;
		int minRow;
		int minCol;
		int maxRow;
		int maxCol;
		
		Property() {
			label;
			noOfPixels;
			minRow;
			minCol;
			maxRow;
			maxCol;
		}	
	};
	
	Property *propertyAry;
	
	CCProperty(ifstream &fileBB) {
		
		fileBB >> maxCC;
		
		propertyAry = new Property[maxCC + 1];
		
		int temp;
		
		for(int i = 1; i <= maxCC; i++) {
			
			fileBB >> temp;
			propertyAry[i].label = temp;
			
			fileBB >> temp;
			propertyAry[i].minRow = temp;
			
			fileBB >> temp;
			propertyAry[i].minCol = temp;
			
			fileBB >> temp;
			propertyAry[i].maxRow = temp;
			
			fileBB >> temp;
			propertyAry[i].maxCol = temp;
			
			fileBB >> temp;
			propertyAry[i].noOfPixels = temp;
		}
	}
		
};

class chainCode {
	
	public:
		int currentCC;
		int minRowOffset;
		int minColOffset;
		int maxRowOffset;
		int maxColOffset;
		struct Point { int row; int col; };
		Point neighborCoord[8];
		int nextDirTable[8]; // hardcoded
		Point currentP;
		Point nextP;
		int nextQ;
		int pChain;
		int lastQ;
		Point startP;
	
	chainCode(int label, int minR, int minC, int maxR, int maxC) {

		currentCC = label;
		minRowOffset = minR;
		minColOffset = minC;
		maxRowOffset = maxR;
		maxColOffset = maxC;
		lastQ = 4;
	
		//hard coding nextDirTable
		nextDirTable[0] = 6;
		nextDirTable[1] = 0;
		nextDirTable[2] = 0;
		nextDirTable[3] = 2;
		nextDirTable[4] = 2;
		nextDirTable[5] = 4;
		nextDirTable[6] = 4;
		nextDirTable[7] = 6;
	}
	
	void loadNeighborCoord(Point currentP) {
		
		neighborCoord[0].row = currentP.row;
		neighborCoord[0].col = currentP.col+1;
		
		neighborCoord[1].row = currentP.row-1;
		neighborCoord[1].col = currentP.col+1;
		
		neighborCoord[2].row = currentP.row-1;
		neighborCoord[2].col = currentP.col;
		
		neighborCoord[3].row = currentP.row-1;
		neighborCoord[3].col = currentP.col-1;
		
		neighborCoord[4].row = currentP.row;
		neighborCoord[4].col = currentP.col-1;
		
		neighborCoord[5].row = currentP.row+1;
		neighborCoord[5].col = currentP.col-1;
		
		neighborCoord[6].row = currentP.row+1;
		neighborCoord[6].col = currentP.col;
		
		neighborCoord[7].row = currentP.row+1;
		neighborCoord[7].col = currentP.col+1;
	}
	
	bool isEqual(Point a, Point b) {
		if ((a.row == b.row) && (a.col == b.col)) {
			return true;
		}
		return false;
	}
	
	int findNextP(Point currentP, int nextQ, int currentCC, int** zeroAry) {
		
		loadNeighborCoord(currentP);
		
		int chainDir = -1;
		
		for(int i = 0; i < 7; i++) {
			if(zeroAry[neighborCoord[(nextQ+i)%8].row][neighborCoord[(nextQ+i)%8].col] == currentCC) {
				chainDir = (nextQ+i)%8;
				nextP = neighborCoord[chainDir];
				break;
			}
		}
		return chainDir;
	}
	
};

int main(int argc, char*argv[]) {

	ifstream fileCC(argv[1]);
	ifstream fileBB(argv[2]);
	ofstream myoutfile1(argv[3], ios::app);
	ofstream myoutfile2(argv[4], ios::app);

	int row, col, max, min;
	fileCC >> row >> col >> min >> max;
	
	myoutfile2 << "Total Rows: " << row << endl;
	myoutfile2 << "Total Cols: " << col << endl;
	myoutfile2 << "Min Value: " << min << endl;
	myoutfile2 << "Max Value: " << max << endl << endl;
		
	Image cc(row, col, min, max);
	cc.zeroFramed();
	cc.loadImage(fileCC);
	 
	CCProperty ccp(fileBB);	
	
	for(int i = 1; i <= ccp.maxCC; i++) {
		
		myoutfile2 << "Process Bounding Box for " << i << endl;
			
		chainCode myChainCode (	ccp.propertyAry[i].label,
							  	ccp.propertyAry[i].minRow, 
							  	ccp.propertyAry[i].minCol,
							  	ccp.propertyAry[i].maxRow,
							  	ccp.propertyAry[i].maxCol);
		
		bool startFlag = true;
		
		for(int row = myChainCode.minRowOffset+1; row <= myChainCode.maxRowOffset+1 && startFlag; row++) {
			for(int col = myChainCode.minColOffset+1; col <= myChainCode.maxColOffset+1 && startFlag; col++) {
				
				if(cc.zeroFramedAry[row][col] == myChainCode.currentCC) {

					myChainCode.startP.row = row;
					myChainCode.startP.col = col;
					
					myoutfile1 << row << " " << col << " " << myChainCode.currentCC << " ";

					myoutfile2 << "Start Row: " << row << endl;
					myoutfile2 << "Start Col: " << col << endl;
					
					myChainCode.currentP.row = row;
					myChainCode.currentP.col = col;
					
					myChainCode.nextP.row = -1;
					myChainCode.nextP.col = -1;

					startFlag = false;
				}
			}
		}	

		while (	!(myChainCode.isEqual(myChainCode.nextP, myChainCode.startP) ) ) {
								
			myChainCode.nextQ = (((myChainCode.lastQ)+1)%8);
			myChainCode.pChain = myChainCode.findNextP(myChainCode.currentP, myChainCode.nextQ, myChainCode.currentCC, cc.zeroFramedAry);
			
			myoutfile1 << myChainCode.pChain << " ";
						
			if(myChainCode.pChain == 0) myChainCode.pChain = 8;
			myChainCode.lastQ = myChainCode.nextDirTable[myChainCode.pChain-1];
			myChainCode.currentP = myChainCode.nextP;
		}
			myoutfile1 << endl;	
			myoutfile2 << endl;
	}
		
	fileCC.close();
	fileBB.close();
	myoutfile1.close();	// close output file
	myoutfile2.close();	// close output file
}

