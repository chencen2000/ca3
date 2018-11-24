 cuda
 dxva2
 qsv
 d3d11va

        public static Bitmap ReadBitmapFromPPM(string file)
        {
            var reader = new BinaryReader(new FileStream(file, FileMode.Open));
            if (reader.ReadChar() != 'P' || reader.ReadChar() != '6')
                return null;
            reader.ReadChar(); //Eat newline
            string widths = "", heights = "";
            char temp;
            while ((temp = reader.ReadChar()) != ' ')
                widths += temp;
            while ((temp = reader.ReadChar()) >= '0' && temp <= '9')
                heights += temp;
            if (reader.ReadChar() != '2' || reader.ReadChar() != '5' || reader.ReadChar() != '5')
                return null;
            reader.ReadChar(); //Eat the last newline
            int width = int.Parse(widths),
                height = int.Parse(heights);
            Bitmap bitmap = new Bitmap(width, height);
            //Read in the pixels
            for (int y = 0; y < height; y++)
                for (int x = 0; x < width; x++)
                    bitmap.SetPixel(x, y, Color.FromArgb(reader.ReadByte(), reader.ReadByte(), reader.ReadByte()));
                    //bitmap.SetPixel(x, y, new Bitmap.Color()
                    //{
                    //    Red = reader.ReadByte(),
                    //    Green = reader.ReadByte(),
                    //    Blue = reader.ReadByte()
                    //});
            return bitmap;
        }
        static void test_ppm()
        {
            string folder = @"C:\projects\local\ca3\ca3";
            foreach(string fn in System.IO.Directory.GetFiles(folder, "*.ppm"))
            {
                ReadBitmapFromPPM(fn).Save("temp_1.jpg");
            }
        }