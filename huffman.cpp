#include <bits/stdc++.h>
using namespace std;

class huffmanTreeComponent{
public:
	int pixel;
	float prob;
	bool isLeaf;
	string code;
	huffmanTreeComponent *left,*right;
	huffmanTreeComponent(int pixel){
		this->pixel = pixel;
		left=nullptr;
		right=nullptr;
		isLeaf=true;
	}
};

class huffmanTree{
public:
	huffmanTreeComponent *root;
	huffmanTree(huffmanTreeComponent *root){
		this->root = root;
	}
	void levelOrder(){
		deque<huffmanTreeComponent*>q;
		if(root){
			q.push_back(root);
		}
		while(!q.empty()){
			if(q.front()->isLeaf){
				cout<<q.front()->pixel<<" ";
			}else{
				cout<<"parent node "; 
			}
			if(q.front()->left){
				q.push_back(q.front()->left);
			}
			if(q.front()->right){
				q.push_back(q.front()->right);
			}
			q.pop_front();
		}
	}
};

bool cmp(huffmanTreeComponent *a, huffmanTreeComponent *b){
	return a->prob>b->prob;
}

class huffmanUtils
{
	int **image;
	int height, breadth,nodes;
	string codeMap[256];
public:
	vector<huffmanTreeComponent*> arr;
	huffmanTreeComponent *root;
	int pixelIntensityMap[256];
	huffmanUtils(){
		for(int i=0;i<256;i++)pixelIntensityMap[i]=0;
		nodes=0;
		for(int i=0;i<256;i++)codeMap[i]="";
	}

	void readImage(char fileName[])
	{
		int i, j;
		int data = 0, fileSeekOffset, bpp = 0;
		long sizeBMP = 0, BMPdataOff = 0;
		int temp = 0;
		FILE *image_file;
		image_file = fopen(fileName, "rb");
		if (image_file == nullptr)
		{
			printf("Error Opening File!!");
			exit(1);
		}
		else
		{
			fileSeekOffset = 2;
			fseek(image_file, fileSeekOffset, SEEK_SET);
			fread(&sizeBMP, 4, 1, image_file);
			fileSeekOffset = 10;
			fseek(image_file, fileSeekOffset, SEEK_SET);
			fread(&BMPdataOff, 4, 1, image_file);
			fseek(image_file, 18, SEEK_SET);
			fread(&breadth, 4, 1, image_file);
			fread(&height, 4, 1, image_file);
			fseek(image_file, 2, SEEK_CUR);
			fread(&bpp, 2, 1, image_file);
			fseek(image_file, BMPdataOff, SEEK_SET);
			image = new int *[height];
			for (i = 0; i < height; i++)
			{
				image[i] = new int[breadth];
			}
			int numberOfBytes = (sizeBMP - BMPdataOff) / 3;
			for (i = 0; i < height; i++)
			{
				for (j = 0; j < breadth; j++)
				{
					fread(&temp, 3, 1, image_file);
					temp = temp & 0x0000FF;
					image[i][j] = temp;
				}
			}
		}
	}

	int *getMapPixel()
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < breadth; j++)
			{
				pixelIntensityMap[image[i][j]]++;
			}
		}
		return pixelIntensityMap;
	}

	void intializeNodes(){
		for(int i=0;i<256;i++){
			if(pixelIntensityMap[i]!=0)
				nodes++;
		}
	}
	
	void initializeArr(){
		int totalPixels = height*breadth;
		for(int i=0;i<256;i++){
			if(pixelIntensityMap[i]!=0){
				huffmanTreeComponent *h = new huffmanTreeComponent(i);
				float probability = (float)pixelIntensityMap[i]/totalPixels;
				h->prob = probability;
				arr.push_back(h);
			}
		}
	}

	huffmanTreeComponent *combineNodes(huffmanTreeComponent *a, huffmanTreeComponent *b){
		huffmanTreeComponent *newNode = new huffmanTreeComponent(a->pixel+b->pixel);
		newNode->isLeaf=false;
		newNode->prob = a->prob+b->prob;
		newNode->left = a;
		newNode->right = b;
		return newNode;
	}
	
	huffmanTreeComponent* createTree(){
		while(arr.size()>1){
			sort(arr.begin(),arr.end(),cmp);
			huffmanTreeComponent *n = combineNodes(arr[arr.size()-2],arr[arr.size()-1]);
			arr.pop_back();
			arr.pop_back();
			arr.push_back(n);
		}
		root = arr[0];
		return root;
	}
	void assignCodeToTreeNodes(huffmanTreeComponent *rootNode, string src){
		if(root!=nullptr){
			if(rootNode->isLeaf){
				rootNode->code = src;
			}
			else{
				assignCodeToTreeNodes(rootNode->left,src+'0');
				assignCodeToTreeNodes(rootNode->right,src+'1');
			}
		}
	}

	void addInCodeMap(huffmanTreeComponent *rootNode){
		if(rootNode){
			if(rootNode->isLeaf){
				codeMap[rootNode->pixel]=rootNode->code;
			}
			else{
				addInCodeMap(rootNode->left);
				addInCodeMap(rootNode->right);
			}
		}
	}
	void encodeImage(){
		ofstream out("encoded_image.txt");
		for(int i=0;i<height;i++){
			for(int j=0;j<breadth;j++){
				out<<codeMap[image[i][j]];
			}
		}
		out.close();
		cout<<"Successfully encoded Image\n";
	}
	void displayCodes(){
		for(int i=0;i<256;i++){
			if(codeMap[i].size()>0)
				cout<<i<<": "<<codeMap[i]<<"\n";
		}
	}
};

int main()
{
	//reading image file
	huffmanUtils util;
	char fileName[]="Input_Image.bmp";
	util.readImage(fileName);
	//creating map for pixel intensities
	int *map = util.getMapPixel();
	// for(int i=0;i<256;i++)cout<<i<<" "<<map[i]<<endl;
	//initialising nodes and the array
	util.intializeNodes();
	util.initializeArr();
	//tree creation
	huffmanTreeComponent *root =  util.createTree();
	util.assignCodeToTreeNodes(root,"");
	util.addInCodeMap(root);
	util.displayCodes();
	util.encodeImage();
	return 0;
}