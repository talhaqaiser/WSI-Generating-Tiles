clc
clear 
close all
%--------------------------------------------------------------------
% WSI directory with mask directory
wsiPath = 'D:\Osaka\WSIs\training_WSIs\part1\';
bgPath =  'C:\Users\BiaLab\Dropbox\mres\Persistant_Homology_Pak\osaka_dataset\Dataset\WSIs_5X\';
outDir = 'C:\Users\BiaLab\Dropbox\mres\Persistant_Homology_Pak\osaka_dataset\dataset_prep\Train\';

t_patches =  750;
n_patches =  750; 
th =  240;

patch_size =  256;

res_lev =  1; % 20X
%--------------------------------------------------------------------
% load WSIs from directory
wsiDir = dir([wsiPath '*.ndpi']);
bgDir = dir([bgPath '*.png']);

for wsiInd = 1:length(wsiDir)
    strcat(num2str(wsiInd) , '/' , num2str(length(wsiDir)))
    
    disp ('Tumor Patches')
    tumor_patches = generateTumorPatches(wsiPath, bgPath, wsiDir(wsiInd).name, t_patches, res_lev, patch_size);
    
    disp ('Normal Patches')
    normal_patches = generateNormalPatches(wsiPath, bgPath, wsiDir(wsiInd).name, n_patches, th, res_lev, patch_size);
    
    disp ('Store Patches')
    flag =  storePatches(outDir, wsiDir(wsiInd).name, tumor_patches, normal_patches);     
    
end
