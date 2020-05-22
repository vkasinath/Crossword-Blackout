#ifndef __MYCLASS_H
#define __MYCLASS_H

class Words {
public:


	Words(const std::string &inword, const std::string &intype, const unsigned int &inlen, const unsigned int &inx, const unsigned int &iny){
		word = inword;
		wtype = intype;
		wlen = inlen;
		wx = inx;
		wy = iny;
	}

	//REPRESENTATION
	std::string word;
	std::string wtype;
	unsigned int wlen;
	unsigned int wx;
	unsigned int wy;

	// Comparions operator
  	bool operator==(const Words &w2) const {
   		 return (
            this->wtype == w2.wtype &&
            this->word  == w2.word  &&
            this->wx    == w2.wx    &&
            this->wy    == w2.wy); 
  }

	void print(){
		std::cout << "word = " << word << " ";
		std::cout << "type = " << wtype << " ";
		std::cout << "len = " << wlen << " ";
		std::cout << "pos = (" << wx << "," << wy << ")";
		std::cout << std::endl;
	}

};

#endif