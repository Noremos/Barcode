using BarWrapperUtility;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;

namespace Parcsh
{
    class Program
    {
        static void computePath()
        {
            pathToSource = Environment.CurrentDirectory;
            string folder = "PrjBarlib";
            while (!Directory.Exists(Path.Combine(pathToSource, folder)))
            {
                pathToSource = Path.GetFullPath(Path.Combine(pathToSource, @"..\"));
                if (Path.GetPathRoot(pathToSource) == pathToSource)
                {
                    return;
                }
            }
            pathToSource = Path.Combine(pathToSource, folder + "/include");
        }

        static string pathToSource;
        static void Main(string[] args)
        {

            string prepFilePath = "D:\\Programs\\C++\\Barcode\\PrjBarlib\\modules\\Barcode.ixx.pre";

            ModuleGenerator d = new ModuleGenerator();
            d.prepFile(prepFilePath);
            return;

            computePath();

            //string[] fileblacklist = { "Component.h", "Hole.h", "include_cv.h", "include_py.h0", "" };
            string[] classesBlackList = { "BarMat", "BarNdarray" };
            string[] whitelist = { "barclasses.h", "barImg.h", "barline.h", "barscalar.h", "barstrucs.h", "barcodeCreator.h" };

            CWrapperPrinter reader = new CWrapperPrinter();
            foreach (var filename in whitelist)
            {
                string filepath = Path.Combine(pathToSource, filename);
                reader.analysFile(filepath);
            }

            reader.printWrapperClasses();
        }
    }
}
