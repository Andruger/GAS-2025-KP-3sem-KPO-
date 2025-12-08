#include <iostream>
#include <time.h>

extern "C" {
    int __stdcall slength(char* buffer, char* str) //   
    {
        if (str == nullptr)
            return 0;
        int len = 0;
        for (int i = 0; i < 256; i++)
            if (str[i] == '\0')
            {
                len = i; break;
            }
        return len;
    }

    short __stdcall aton(char* buffer, char* ptr) //    
    {
        short n = 0;
        while (*ptr >= '0' && *ptr <= '9') {
            n *= 10;
            n += *ptr++;
            n -= '0';
        }
        return n;

    }

    int __stdcall outrad(char* ptr) //   /
    {
        if (ptr == nullptr)
        {
            std::cout << std::endl;
        }
        for (int i = 0; ptr[i] != '\0'; i++)
            std::cout << ptr[i];
        return 0;
    }

    char* __stdcall concat(char* buffer, char* str1, char* str2) //  
    {
        int i = 0;
        for (int j = 0; str1[j] != '\0'; j++)
        {
            if (i == 255)
                break;
            buffer[i++] = str1[j];
        }
        for (int j = 0; str2[j] != '\0'; j++)
        {
            if (i == 255)
                break;
            buffer[i++] = str2[j];
        }
        buffer[i] = '\0';
        return buffer;
    }

    int __stdcall compare(char* ptr, char* a, char* b) // 
    {
        if (strcmp(a, b) < 0)
            return 0;
        if (strcmp(a, b) == 0)
            return 1;
        if (strcmp(a, b) > 0)
            return 2;
        return 0;
    }

    short __stdcall rnd(char* ptr, short a, short b) //
    {
        srand(time(NULL));
        short res;
        res = (short)(rand() % b + a);
        return res;
    }

    void _pause() //   
    {
        system("pause");
    }

    int __stdcall outlich(short value) //   /
    {
        std::cout << value;
        return 0;
    }
}
