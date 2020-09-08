function test()
x = 1;
y = 1;

[file,path] = uigetfile('*.*');

dodo = imread(strcat(path, file));
dodo = imresize(dodo, [64 128]);
dodo = imbinarize(dodo);

fid=fopen('image.c','w');

fprintf(fid, "static const ssd1306_color_t image_data [] = { \n\r");

for(x = 1:64)
      for(y = 1:128)
        fprintf(fid, "%d, ", dodo(x,y));
      end
end

fprintf(fid, "\n\r };");

fclose(fid);

end