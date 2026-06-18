#include <iostream>
#include <string>
#include <vector>
#include<windows.h>


#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

const unsigned char DOTs[] = { 0x01,0x02,0x04,0x08   ,0x10,0x20,0x40,0x80 };

void threshold(std::vector<unsigned char>& img,int threshold){
    for (size_t i = 0; i < img.size(); ++i) {
        img[i] = (img[i] > threshold) ? 255 : 0;
    }
}

uint8_t bit_index(int y, int x) {
    
    

    // map pixels to Braille bits
    if (y==0 && x==0) return (0); // dot 1
    if (y==1 && x==0) return (1); // dot 2
    if (y==2 && x==0) return (2); // dot 3
    if (y==0 && x==1) return (3); // dot 4
    if (y==1 && x==1) return (4); // dot 5
    if (y==2 && x==1) return (5); // dot 6
    if (y==3 && x==0) return (6); // dot 7
    if (y==3 && x==1) return (7); // dot 8

    //return static_cast<char32_t>(0x2800 + dots);
}
std::string utf8_encode(uint32_t codepoint) {
    // all Braille points are in U+2800–U+28FF, always 3 bytes in UTF-8
    std::string out;
    out += static_cast<char>(0xE0 | ((codepoint >> 12) & 0x0F));
    out += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
    out += static_cast<char>(0x80 | (codepoint & 0x3F));
    return out;
}
std::string byteImage(std::vector<unsigned char> img,int char_cols, int char_rows ,int thresh){
    
    std::string byteImageStr;
    
    for(int row=0; row<char_rows*4; row+=4){
        
        for (int col=0;col<char_cols*2; col+=2){
            uint8_t byte = 0;

            for (int dy = 0; dy < 4; ++dy) {
            for (int dx = 0; dx < 2; ++dx) {

                if( img[(row + dy) * char_cols*2 + (col + dx)] > thresh)
                    byte |= (1 << bit_index(dy,dx));
                
            }}
            byteImageStr += utf8_encode(0x2800 + byte);
            
        }
        byteImageStr += '\n';
    }
    return byteImageStr;
}


int main(int argc, char* argv[])
{

    #ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8); // Enable UTF-8 output on Windows
    #endif

    int char_cols = 40, char_rows = 20;
    std::string imgPath = "";
    if (argc > 1 && std::string(argv[1]) == "--help")
    {
        std::cout
            << "Usage:\n"
            << "  imgconsole <image_path> [cols rows]\n"
            << "Examples:\n"
            << "  imgconsole cat.png\n"
            << "  imgconsole cat.png 80 40\n";
        return 0;
    }
    if (argc >= 2)
    {
        imgPath = argv[1];
        if (argc >= 4){
            try
            {
                char_cols = std::stoi(argv[2]);
                char_rows = std::stoi(argv[3]);
            }
            catch (...)
            {
                std::cerr << "Invalid dimensions.\n";
                return 1;
            }
        }
    }
    else
    {
        std::cout<< "Enter Image Path: ";
        std::cin>> imgPath;
    }

    int width,height,channel;
    unsigned char* data = stbi_load(imgPath.c_str(), &width, &height, &channel, 1);

    if(!data)
    {
        std::cerr<< "Failed to load image: " << imgPath << std::endl;
        return 1;
    }

    std::cout << "\n Width : "<< width;
    std::cout << "\n Height : "<< height;
    std::cout << "\n Channel : "<< channel ;

    std::vector<unsigned char> img(char_rows*2 * char_cols*4);
    stbir_resize_uint8_srgb(data, width, height, 0, 
                            img.data(),  char_cols*2, char_rows*4 ,  0, STBIR_1CHANNEL);

    stbi_image_free(data);

    threshold(img,128);
    std::string out =  byteImage(img,char_cols,char_rows,98);
    std::cout << std::endl << out << std::endl;

    


return 0;
}
