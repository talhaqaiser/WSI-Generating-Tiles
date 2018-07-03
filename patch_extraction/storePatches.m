function [flag, count] =  storePatches(outDir, wsiName, tumor_patches, normal_patches, count)
    wsiName =  wsiName(1:end-5);
    mkdir(wsiName);
    tum_path = strcat(outDir, wsiName, '\Tumor\');
    nor_path = strcat(outDir, wsiName,'\Normal\');
    mkdir(tum_path); mkdir(nor_path);

    for i=1:size(tumor_patches,2)
        imwrite(cell2mat(tumor_patches(i)), strcat(tum_path, wsiName, '_',  num2str(count), '_t.png'));        
        count = count + 1;
    end
    
    for i=1:size(normal_patches,2)
        imwrite(cell2mat(normal_patches(i)), strcat(nor_path, wsiName, '_',  num2str(count), '_n.png'));     
        count =  count +1;
    end
    
    flag =  1;
    count;
end