clc
clear 
close all
%--------------------------------------------------------------------
% WSI directory with mask directory
wsiPath = 'D:\Osaka\WSIs\delete_this\';
bgPath =  'C:\Users\BiaLab\Dropbox\mres\Persistant_Homology_Pak\osaka_dataset\Dataset\WSIs_5X\';
outDir = 'D:\HnE-feature-extractor-src\dataset_patches\';

t_patches =  250;
n_patches =  250; 
th =  240;

patch_size =  [256,320, 384, 448, 512];

res_lev =  0; % 20X
%--------------------------------------------------------------------------
% load WSIs from directory
wsiDir = dir([wsiPath '*.ndpi']);
bgDir = dir([bgPath '*.png']);
csv_file = 'dataset_patch_info.csv';
fid = fopen(csv_file,'a');
%--------------------------------------------------------------------------
count = 0;
for wsiInd = 1:length(wsiDir)
    strcat(num2str(wsiInd) , '/' , num2str(length(wsiDir)))
    count =0;
    for p = 1:length(patch_size)
        disp ('Tumor Patches')
        tumor_patches = generateTumorPatches(wsiPath, bgPath, wsiDir(wsiInd).name, t_patches, res_lev, patch_size(p), fid);

        disp ('Normal Patches')
        normal_patches = generateNormalPatches(wsiPath, bgPath, wsiDir(wsiInd).name, n_patches, th, res_lev, patch_size(p), fid);

        disp ('Store Patches')bi
        [flag, count] =  storePatches(outDir, wsiDir(wsiInd).name, tumor_patches, normal_patches, count);     
    end
end
fid = fclose(fid);
