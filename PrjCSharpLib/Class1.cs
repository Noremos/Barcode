using System;
using System.Drawing;
namespace BarSharp
{
    public class Class1
    {
        static string c = @"
#include <stdio.h>

int main(void) 
{
    printf(^Hello, World!\n^);
    return 0;
}
".Replace('^', '\"');

        static string cpp = @"
#include <iostream>

int main(int, char**) 
{
    std::cout << ^Hello, World!\n^;
    return 0;
}".Replace('^', '\"');

        static string python = "  print(\"Hello, World!\")";

        static string js = "  console.log(\"Hello, World!\")";

        static string csharp = @"
class HelloWorld
{
    static void Main()
    {
        System.Console.WriteLine(^Hello, World!^);
    }
}".Replace('^', '\"'); //modern

        static string php = @"
<?php
echo ^Hello, World!^;
?>".Replace('^', '\"');

        static string java = @"
class Main
{
    public static void main(String[] args)
    {
        System.out.println(^Hello, World!^);
    }
}".Replace('^', '\"');


        static string go = @"
package main
import ^fmt^

func main()
{
    fmt.Println(^Hello, World!^)
}".Replace('^', '\"');

        static void Main()
        {
            string[] lines = {
                c,
                cpp,
                csharp,
                java,
                go,
                python,
                js,
                php
                };

            int mwid = 0;
            for (int i = 0; i < lines.Length; ++i)
            {
                string line = lines[i] = lines[i].Substring(2);
                Console.WriteLine(line);
                //Console.WriteLine();

                mwid = Math.Max(mwid, line.Length);
            }

            Bitmap mp = new Bitmap(mwid, lines.Length);
            for (int i = 0; i < mp.Height; ++i)
            {
                for (int j = 0; j < lines[i].Length; ++j)
                {
                    byte c = (byte)lines[i][j];
                    mp.SetPixel(j, i, Color.FromArgb(c, c, c));
                }
                string line = lines[i] = lines[i].Substring(2);
                Console.WriteLine(line);
                //Console.WriteLine();

                mwid = Math.Max(mwid, line.Length);
            }
            mp.Save(@"D:\Programs\C++\Barcode\PrjCSharpLib\result.bmp");
        }
    }
}
