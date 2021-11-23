#include <bits/stdc++.h>
using namespace std;

//single huffman node component
class huffmanTreeComponent
{
public:
	//pixel represents the pixel intensity of that pixel.
	int pixel; 

	// probability of that pixel intensity in the image
	float prob;

	//represents if that pixel is a leaf or not.
	bool isLeaf;

	//huffman code of that node
	string code;

	//left and right components of that tree.
	huffmanTreeComponent *left, *right;

	// parameterized constructor
	// it sets the pixel value and by default that node is a leaf.
	huffmanTreeComponent(int pixel)
	{
		this->pixel = pixel;
		left = nullptr;
		right = nullptr;
		isLeaf = true;
	}
};

//compare huffman trees by their probability
class compareHuffmanComponent{
    public:
    bool operator()(huffmanTreeComponent *a, huffmanTreeComponent *b){
        return a->prob>b->prob;
    }
};

//comparsion function for sorting nodes.
//It compares according to pixel probability.
bool cmp(huffmanTreeComponent *a, huffmanTreeComponent *b)
{
	return a->prob > b->prob;
}

// utlity functions for encoding the image.
class huffmanCodingUtils
{
	// represents the pixel intensity graph of an image.
	// It is stored in a 2D array.
	int **image;

	//height - height of image
	//breadth - breadth of image
	//nodes - total nodes available.
	int height, breadth;

	//stores the huffmanCodes of all pixel intensities.
	string codeMap[256];

	//stores size of encoded image
	long long encodedSize;
public:

	// array that stores all pixel intensity nodes
	vector<huffmanTreeComponent *> arr;

	//defines the master root of the tree.
	huffmanTreeComponent *root;

	//stores the pixel intensities of each pixel.
	int pixelIntensityMap[256];

	//default constructor
	huffmanCodingUtils()
	{
		// initializes pixel intensities to 0.
		for (int i = 0; i < 256; i++)
			pixelIntensityMap[i] = 0;
		//codes to 0
		for (int i = 0; i < 256; i++)
			codeMap[i] = "";
		encodedSize=0;
	}

	//accessor function that returns height
	int getHeight()
	{
		return height;
	}

	//accessor function that returns breadth
	int getBreadth()
	{
		return breadth;
	}

	//function to read the image.
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
			//seeking to the beginning of the bmp file
			fseek(image_file, fileSeekOffset, SEEK_SET);
			//getting the size of bmp file 
			fread(&sizeBMP, 4, 1, image_file);
			fileSeekOffset = 10;
			//seeking to offset where pixel array is stored
			fseek(image_file, fileSeekOffset, SEEK_SET);
			//getting BMP data offset and inserting it
			fread(&BMPdataOff, 4, 1, image_file);
			//seeking to the pixel array again
			fseek(image_file, 18, SEEK_SET);
			//getting the height and breadth of image
			fread(&breadth, 4, 1, image_file);
			fread(&height, 4, 1, image_file);
			fseek(image_file, 2, SEEK_CUR);
			//reading no of bits per pixel
			fread(&bpp, 2, 1, image_file);

			fseek(image_file, BMPdataOff, SEEK_SET);
			// dynamically allocating image space for insertion
			image = new int *[height];
			for (i = 0; i < height; i++)
			{
				image[i] = new int[breadth];
			}
			for (i = 0; i < height; i++)
			{
				for (j = 0; j < breadth; j++)
				{
					fread(&temp, 3, 1, image_file);
					// taking and with hexadecimal FF -> (16^1+16^0)
					temp = temp & 0x0000FF;
					image[i][j] = temp;
				}
			}
		}
	}

	//accessor function that updates the pixelIntensity map using image pixels
	//using  and returns the pixelIntensity map
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
	
	// adds all the non zero pixelintensities in an array.
	void initializeArr()
	{
		int totalPixels = height * breadth;
		for (int i = 0; i < 256; i++)
		{
			if (pixelIntensityMap[i] != 0)
			{
				huffmanTreeComponent *h = new huffmanTreeComponent(i);
				float probability = (float)pixelIntensityMap[i] / totalPixels;
				h->prob = probability;
				arr.push_back(h);
			}
		}
	}

	//combines two nodes of the huffman tree.
	huffmanTreeComponent *combineNodes(huffmanTreeComponent *a, huffmanTreeComponent *b)
	{
		huffmanTreeComponent *newNode = new huffmanTreeComponent(a->pixel + b->pixel);
		newNode->isLeaf = false;
		newNode->prob = a->prob + b->prob;
		newNode->left = a;
		newNode->right = b;
		return newNode;
	}

	//creates the huffman tree 
	huffmanTreeComponent *createTree()
	{
		while (arr.size() > 1)
		{
			sort(arr.begin(), arr.end(), cmp);
			huffmanTreeComponent *n = combineNodes(arr[arr.size() - 2], arr[arr.size() - 1]);
			arr.pop_back();
			arr.pop_back();
			arr.push_back(n);
		}
		root = arr[0];
		return root;
	}

	// heap optimization 
	huffmanTreeComponent *createTreeTimeEff()
	{
		priority_queue<huffmanTreeComponent* ,vector<huffmanTreeComponent*>, compareHuffmanComponent> q;
		for(int i=0;i<arr.size();i++){
			q.push(arr[i]);
		}
		while(q.size()>1){
			huffmanTreeComponent* a = q.top();
			q.pop();
			huffmanTreeComponent *b = q.top();
			q.pop();
			huffmanTreeComponent *n = combineNodes(a,b);
			q.push(n);
		}
		root = q.top();
		return  root;
	}

	// backtracks and assigns code to tree nodes according to their positions
	void assignCodeToTreeNodes(huffmanTreeComponent *rootNode, string src)
	{
		if (rootNode != nullptr)
		{
			if (rootNode->isLeaf)
			{
				rootNode->code = src;
				codeMap[rootNode->pixel] = rootNode->code;
			}
			else
			{
				assignCodeToTreeNodes(rootNode->left, src + '0');
				assignCodeToTreeNodes(rootNode->right, src + '1');
			}
		}
	}

	//encodes the final image in a txt file
	void encodeImage()
	{
		ofstream out("encoded_image.txt");
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < breadth; j++)
			{
				encodedSize+=codeMap[image[i][j]].size();
				for(int k=0;k<codeMap[image[i][j]].size();k++){
					bool code = (codeMap[image[i][j]][k]-'0');
					out << code;
				}
			}
		}
		out.close();
		cout << "Successfully encoded Image\n";
	}

	//utility function that displays code.
	void displayCodes()
	{
		for (int i = 0; i < 256; i++)
		{
			if (codeMap[i].size() > 0)
				cout << i << ": " << codeMap[i] << "\n";
		}
	}

	//display encoded images' size
	void displayEncodedSize(){
		cout<<"The encoded image has a size of "<<encodedSize<<" bits\n";
		cout<<"While the original size is: "<<height*breadth*8<<" bits\n";
	}

};

// utils for decoding the image.
class huffmanDecodingUtils
{
	//root node of huffman tree
	huffmanTreeComponent *root;
	// represents the pixel intensity graph of an image.
	// It is stored in a 2D array.
	int **image;
	//height- height of image
	//breadth-breadth of image
	int height, breadth;

public:
	//retrieves the tree height and breadth from coding utilities.
	void getDimensions(huffmanCodingUtils cUtil)
	{
		height = cUtil.getHeight();
		breadth = cUtil.getBreadth();
		cout << "Got dimensions successfully\n";
	}

	// initializes the image with provided breadth and height.
	void initializeImage()
	{
		image = new int *[height];
		for (int i = 0; i < height; i++)
		{
			image[i] = new int[breadth];
		}
		cout << "initialized image successfully\n";
	}

	//decodes the image using the root 
	void decodeImage(huffmanTreeComponent *root)
	{
		int track = 0;
		ifstream encodedImage("encoded_image.txt");
		if (!encodedImage.is_open())
		{
			cout << "Couldn't open the file-"
				 << "encoded_image.txt"
				 << "\n";
		}
		else
		{
			cout << "Decoding image data\n";
			char c;
			int i = 0, j = 0;
			huffmanTreeComponent *temp = root;
			while (encodedImage.get(c))
			{
				if (c=='0')
				{
					temp = temp->left;
					if (temp && temp->isLeaf)
					{
						image[i][j] = temp->pixel;
						i = i + (j + 1) / breadth;
						j = (j + 1) % breadth;
						temp = root;
					}
				}
				else
				{
					temp = temp->right;
					if (temp && temp->isLeaf)
					{
						image[i][j] = temp->pixel;
						i = i + (j + 1) / breadth;
						j = (j + 1) % breadth;
						temp = root;
					}
				}
			}
		}
	}

	//displays the pixels of image decoded.
	void displayImagePixels()
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < breadth; j++)
			{
				cout << setw(3) << image[i][j] << " ";
			}
			cout << "\n";
		}
	}
	
};

class Application{
	huffmanCodingUtils cutil;
	huffmanDecodingUtils deutil;
	void renderMenu();
	void renderInputMenu(char filename[]);
	void renderSampleMenu();
	void renderDefault();
	void renderEncodeMenu();
	void renderDecodeMenu();
	void renderDisplayCodes();
	public:
	void start();
};

int main()
{
	Application app;
	app.start();	

	return 0;
}

void Application::start(){
	system("cls");
	printf("Welcome to huffman encoding and decoding menu\n");
	system("pause");
	renderMenu();
}

void Application::renderMenu(){
	system("cls");
	printf("Enter your encoding preferences\n");
	printf("1. Encode a sample image\n");
	printf("2. Encode a default image\n");
	int ch;
	cin>>ch;
	switch(ch){
		case 1: 
			renderSampleMenu();
			break;
		case 2:
			renderDefault();
			break;
		default:
			printf("Enter an appropriate choice\n");
	}
}

void Application::renderInputMenu(char filename[]){
	cutil.readImage(filename);
	int *map = cutil.getMapPixel();
	cutil.initializeArr();
	cutil.createTreeTimeEff();
	cutil.assignCodeToTreeNodes(cutil.root,"");
}

void Application::renderSampleMenu(){
	system("cls");
	printf("Enter the name of sample file you want to encode\n");
	char filename[100];
	cin>>filename;
	renderInputMenu(filename);
	printf("Press a key to encode\n");
	system("pause");
	renderEncodeMenu();
	system("pause");
	renderDisplayCodes();
	printf("Do you want to decode this image?(y/n)\n");
	char ch;
	cin>>ch;
	if(ch=='y'){
		system("cls");
		renderDecodeMenu();
	}
}

void Application::renderDefault(){
	system("cls");
	char fileName[]="Input_Image.bmp";
	renderInputMenu(fileName);
	printf("Press a key to encode\n");
	system("pause");
	renderEncodeMenu();
	system("pause");
	renderDisplayCodes();
}

void Application::renderEncodeMenu(){
	system("cls");
	cutil.encodeImage();
	cutil.displayEncodedSize();
} 

void Application::renderDisplayCodes(){
	system("cls");
	printf("Huffman codes for the given image\n");
	cutil.displayCodes();
	return;
}

void Application::renderDecodeMenu(){
	deutil.getDimensions(cutil);
	deutil.initializeImage();
	deutil.decodeImage(cutil.root);
	deutil.displayImagePixels();
}