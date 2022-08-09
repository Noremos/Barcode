using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;

namespace Parcsh
{
    class Program
    {
        static void Main(string[] args)
        {
            computePath();
            types.Add("invalid", 0);

            foreach (var filename in Directory.GetFiles(pathToSource))
            {
                string filepath = Path.Combine(pathToSource, filename);
                analysFile(filepath);
            }

            printCClasses();
        }
    }
}
