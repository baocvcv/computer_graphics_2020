#ifndef IMAGE_H
#define IMAGE_H

#include "common.hpp"
#include "vec.hpp"

unsigned char ReadByte( FILE* file)
{
    unsigned char b;
    int success = fread( ( void* )( &b ), sizeof( unsigned char ), 1, file );
    assert( success == 1 );
    return b;
}

void WriteByte( FILE* file, unsigned char b )
{
    int success = fwrite( ( void* )( &b ), sizeof( unsigned char ), 1, file );
    assert( success == 1 );
}

// Simple image class
struct Image {
    int width;
    int height;
    Vec3 *data;

    Image() : width(0), height(0), data(nullptr) {}

    Image(int w, int h) {
        width = w;
        height = h;
        data = new Vec3[width * height];
    }

    ~Image() {
        delete[] data;
    }

    void SetSize(int w, int h) {
        assert(w > 0 && h > 0);
        if (data == nullptr)
            delete[] data;
        width = w;
        height = h;
        data = new Vec3[width * height];
    }

    int Width() const {
        return width;
    }

    int Height() const {
        return height;
    }

    const Vec3 &GetPixel(int x, int y) const {
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        return data[y * width + x];
    }

    void SetAllPixels(const Vec3 &color) {
        for (int i = 0; i < width * height; ++i) {
            data[i] = color;
        }
    }

    void SetPixel(int x, int y, const Vec3 &color) {
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        data[y * width + x] = color;
    }

    void IncrementPixel(int x, int y, const Vec3 &color) {
        assert(x >= 0 && x < width);
        assert(y >= 0 && y < height);
        data[y * width + x] += color;
    }

    static Image *LoadPPM(const char *filename) {
        assert(filename != NULL);
        // must end in .ppm
        const char *ext = &filename[strlen(filename)-4];
        assert(!strcmp(ext,".ppm"));
        FILE *file = fopen(filename,"rb");
        // misc header information
        int width = 0;
        int height = 0;  
        char tmp[100];
        fgets(tmp,100,file); 
        assert (strstr(tmp,"P6"));
        fgets(tmp,100,file); 
        assert (tmp[0] == '#');
        fgets(tmp,100,file); 
        sscanf(tmp,"%d %d",&width,&height);
        fgets(tmp,100,file); 
        assert (strstr(tmp,"255"));
        // the data
        Image *answer = new Image(width,height);
        // flip y so that (0,0) is bottom left corner
        for (int y = height-1; y >= 0; y--) {
            for (int x = 0; x < width; x++) {
                unsigned char r,g,b;
                r = fgetc(file);
                g = fgetc(file);
                b = fgetc(file);
                Vec3 color(r/255.0,g/255.0,b/255.0);
                answer->SetPixel(x,y,color);
            }
        }
        fclose(file);
        return answer;
    }

    void SavePPM(const char *filename) const {
        assert(filename != NULL);
        // must end in .ppm
        const char *ext = &filename[strlen(filename)-4];
        assert(!strcmp(ext,".ppm"));
        FILE *file = fopen(filename, "w");
        // misc header information
        assert(file != NULL);
        fprintf (file, "P6\n");
        fprintf (file, "# Creator: Image::SavePPM()\n");
        fprintf (file, "%d %d\n", width,height);
        fprintf (file, "255\n");
        // the data
        // flip y so that (0,0) is bottom left corner
        for (int y = height-1; y >= 0; y--) {
            for (int x=0; x<width; x++) {
                Vec3 v = GetPixel(x,y);
                // fputc (ClampColorComponent(v[0]),file);
                // fputc (ClampColorComponent(v[1]),file);
                // fputc (ClampColorComponent(v[2]),file);
                fputc(gamma_trans(clamp(v.x)), file);
                fputc(gamma_trans(clamp(v.y)), file);
                fputc(gamma_trans(clamp(v.z)), file);
            }
        }
        fclose(file);
    }

    static Image *LoadTGA(const char *filename) {
        assert(filename != NULL);
        // must end in .tga
        const char *ext = &filename[strlen(filename)-4];
        assert(!strcmp(ext,".tga"));
        FILE *file = fopen(filename,"rb");
        // misc header information
        int width = 0;
        int height = 0;
        for (int i = 0; i < 18; i++) {
            unsigned char tmp;
            tmp = ReadByte(file);
            if (i == 2) assert(tmp == 2);
            else if (i == 12) width += tmp;
            else if (i == 13) width += 256*tmp;
            else if (i == 14) height += tmp;
            else if (i == 15) height += 256*tmp;
            else if (i == 16) assert(tmp == 24);
            else if (i == 17) assert(tmp == 32);
            else assert(tmp == 0);
        }
        // the data
        Image *answer = new Image(width,height);
        // flip y so that (0,0) is bottom left corner
        for (int y = height-1; y >= 0; y--) {
            for (int x = 0; x < width; x++) {
                unsigned char r,g,b;
                // note reversed order: b, g, r
                b = ReadByte(file);
                g = ReadByte(file);
                r = ReadByte(file);
                Vec3 color(r/255.0,g/255.0,b/255.0);
                answer->SetPixel(x,y,color);
            }
        }
        fclose(file);
        return answer;
    }

    void SaveTGA(const char *filename) const {
        assert( filename != NULL );
        // must end in .tga
        const char* ext = &filename[ strlen( filename ) - 4 ];
        assert( !strcmp( ext,".tga" ) );
        FILE* file = fopen( filename, "wb" );
        // misc header information
        for( int i = 0; i < 18; i++)
        {
            if (i == 2) WriteByte(file,2);
            else if (i == 12) WriteByte(file,width%256);
            else if (i == 13) WriteByte(file,width/256);
            else if (i == 14) WriteByte(file,height%256);
            else if (i == 15) WriteByte(file,height/256);
            else if (i == 16) WriteByte(file,24);
            else if (i == 17) WriteByte(file,32);
            else WriteByte(file,0);
        }
        // the data
        // flip y so that (0,0) is bottom left corner
        for (int y = height-1; y >= 0; y--)
        {
            for (int x = 0; x < width; x++)
            {
                Vec3 v = GetPixel(x,y);
                // note reversed order: b, g, r
                // WriteByte(file,ClampColorComponent(v[2]));
                // WriteByte(file,ClampColorComponent(v[1]));
                // WriteByte(file,ClampColorComponent(v[0]));
                WriteByte(file, gamma_trans(clamp(v.z)));
                WriteByte(file, gamma_trans(clamp(v.y)));
                WriteByte(file, gamma_trans(clamp(v.x)));
            }
        }
        fclose(file);
    }

    int SaveBMP(const char *filename) {
        struct BMPHeader
        {
            char bfType[3];       /* "BM" */
            int bfSize;           /* Size of file in bytes */
            int bfReserved;       /* set to 0 */
            int bfOffBits;        /* Byte offset to actual bitmap data (= 54) */
            int biSize;           /* Size of BITMAPINFOHEADER, in bytes (= 40) */
            int biWidth;          /* Width of image, in pixels */
            int biHeight;         /* Height of images, in pixels */
            short biPlanes;       /* Number of planes in target device (set to 1) */
            short biBitCount;     /* Bits per pixel (24 in this case) */
            int biCompression;    /* Type of compression (0 if no compression) */
            int biSizeImage;      /* Image size, in bytes (0 if no compression) */
            int biXPelsPerMeter;  /* Resolution in pixels/meter of display device */
            int biYPelsPerMeter;  /* Resolution in pixels/meter of display device */
            int biClrUsed;        /* Number of colors in the color table (if 0, use 
                                    maximum allowed by biBitCount) */
            int biClrImportant;   /* Number of important colors.  If 0, all colors 
                                    are important */
        };

        int i, j, ipos;
        int bytesPerLine;
        unsigned char *line;
        Vec3* rgb = data;
        FILE *file;
        struct BMPHeader bmph;

        /* The length of each line must be a multiple of 4 bytes */

        bytesPerLine = (3 * (width + 1) / 4) * 4;

        strcpy(bmph.bfType, "BM");
        bmph.bfOffBits = 54;
        bmph.bfSize = bmph.bfOffBits + bytesPerLine * height;
        bmph.bfReserved = 0;
        bmph.biSize = 40;
        bmph.biWidth = width;
        bmph.biHeight = height;
        bmph.biPlanes = 1;
        bmph.biBitCount = 24;
        bmph.biCompression = 0;
        bmph.biSizeImage = bytesPerLine * height;
        bmph.biXPelsPerMeter = 0;
        bmph.biYPelsPerMeter = 0;
        bmph.biClrUsed = 0;       
        bmph.biClrImportant = 0; 

        file = fopen (filename, "wb");
        if (file == NULL) return(0);
    
        fwrite(&bmph.bfType, 2, 1, file);
        fwrite(&bmph.bfSize, 4, 1, file);
        fwrite(&bmph.bfReserved, 4, 1, file);
        fwrite(&bmph.bfOffBits, 4, 1, file);
        fwrite(&bmph.biSize, 4, 1, file);
        fwrite(&bmph.biWidth, 4, 1, file);
        fwrite(&bmph.biHeight, 4, 1, file);
        fwrite(&bmph.biPlanes, 2, 1, file);
        fwrite(&bmph.biBitCount, 2, 1, file);
        fwrite(&bmph.biCompression, 4, 1, file);
        fwrite(&bmph.biSizeImage, 4, 1, file);
        fwrite(&bmph.biXPelsPerMeter, 4, 1, file);
        fwrite(&bmph.biYPelsPerMeter, 4, 1, file);
        fwrite(&bmph.biClrUsed, 4, 1, file);
        fwrite(&bmph.biClrImportant, 4, 1, file);
    
        line = (unsigned char *)malloc(bytesPerLine);
        if (line == NULL)
        {
            fprintf(stderr, "Can't allocate memory for BMP file.\n");
            return(0);
        }

        for (i = 0; i < height ; i++)
        {
            for (j = 0; j < width; j++)
            {
                ipos = (width * i + j);
                // line[3*j] = ClampColorComponent(rgb[ipos].z);
                // line[3*j+1] =ClampColorComponent( rgb[ipos].y);
                // line[3*j+2] = ClampColorComponent( rgb[ipos].x);
                line[3*j] = gamma_trans(clamp(rgb[ipos].z));
                line[3*j+1] = gamma_trans(clamp(rgb[ipos].y));
                line[3*j+2] = gamma_trans(clamp(rgb[ipos].x));
            }
            fwrite(line, bytesPerLine, 1, file);
        }

        free(line);
        fclose(file);

        return(1);
    }

    void SaveImage(const char *filename) {
        int len = strlen(filename);
        if(strcmp(".bmp", filename+len-4)==0){
            SaveBMP(filename);
        }else{
            SaveTGA(filename);
        }
    }
};

#endif // IMAGE_H
