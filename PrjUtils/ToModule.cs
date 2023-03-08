using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BarWrapperUtility
{
    using System;
    using System.IO;
    using System.Text.RegularExpressions;

    class ModuleGenerator
    {
        string env = @"D:\Programs\C++\Barcode\PrjBarlib";


        enum Mode
        {
            simple =0,
            files,
            exclude
        };

        List<string> incudes;
        HashSet<string> excludeIncudes;


        string clearinclude(string s)
        {
            s = s.Trim();
            string re = "";
            s = s.Replace("#include", "");
            for (int i = 0; i < s.Length; i++)
            {
                char c = s[i];
                switch (c)
                {
                    case '\'':
                    case '"':
                    case ' ':
                        break;
                    case '\\':
                    case '/':
                        re = "";
                        break;

                    case '.':
                        break;
                    default:
                        re += s[i];
                        break;
                }

                if (c == '.')
                    break;
            }

            return re.ToLower();
        }

        string insertImpls(out string outImpl)
        {
            foreach (var incude in incudes)
            {
                if (incude.TrimEnd().EndsWith(".h"))
                    excludeIncudes.Add(clearinclude(incude));
            }

            string outIncludes = "";
            outImpl = "";

            foreach (var incude in incudes)
            {
                string[] lines = File.ReadAllLines(Path.Combine(env, incude));
                foreach (var line in lines)
                {
                    string prepline = line.Trim();
                    if (prepline.StartsWith("#include"))
                    {
                        if (excludeIncudes.Contains(clearinclude(prepline)))
                            continue;
                        else
                        {
                            string cleard = clearinclude(prepline);
                            if (!excludeIncudes.Contains(cleard))
                            {
                                excludeIncudes.Add(cleard);
                                outIncludes += prepline + Environment.NewLine;
                            }
                        }
                    }
                    else if (prepline != "#pragma once")
                        outImpl += line + Environment.NewLine;
                }
            }

            return outIncludes;
        }

        public void prepFile(string fileName)
        {
            string outModule = "";
            string[] lines = File.ReadAllLines(fileName);
            incudes = new List<string>();
            excludeIncudes = new();

            Mode d = Mode.simple;
            int i = 0;
            string body = "";
            for (; i < lines.Length; i++)  
            { 
                string line = lines[i].Trim();

                if (line == "STRAT_IMPL_FILES")
                {
                    d = Mode.files;
                    continue;
                }
                else if (line == "END_IMPL")
                {
                    d = Mode.simple;
                    outModule += insertImpls(out body);

                    continue;
                }
                else if (line == "EXCLUDE_FILES")
                {
                    d = Mode.exclude;
                    continue;
                }
                else if (line == "END_EXCLUDE")
                {
                    d = Mode.simple;
                    outModule += insertImpls(out body);

                    continue;
                }
                else if (line == "INSERT_SOURCE")
                {
                    outModule += body;
                    continue;
                }

                switch (d)
                {
                    case Mode.simple:
                        outModule += lines[i] + Environment.NewLine;
                        break;
                    case Mode.files:
                        if (line.Length != 0)
                            incudes.Add(line);
                        break;
                    case Mode.exclude:
                        if (line.Length != 0)
                            excludeIncudes.Add(clearinclude(line));
                        break;
                }
            }

            File.WriteAllText(fileName.Replace(".ixx.pre", ".ixx"), outModule);
        }
    }
}
