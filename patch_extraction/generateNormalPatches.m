function normal_patches = generateNormalPatches(wsiPath, bgPath, wsiName, n_patches, th, res_lev, patch_size, fid)
mask_lev = 3;
mask =  imread(strcat(bgPath, wsiName(1:end-5), '_m.png'));

if (mean(mask(:) )==255)
    mask =  mask.*0;
end

% mask pre-processing
mask = maskShrinkBounderies(mask, 'n');

resolution = 0; 
openslide_load_library();
wsiPtr = openslide_open(strcat(wsiPath, wsiName));
wsiLabels = [];
[mppX, mppY, width, height, numberOfLevels, ...
downsampleFactors, objectivePower] = openslide_get_slide_properties(wsiPtr);
wsi_r = height/ (2^resolution);
wsi_c = width/ (2^resolution); 
wsi_lev =  numberOfLevels;
mask  =  imresize(mask, [wsi_r, wsi_c]);

r_list = []; c_list= [];
r_range =  2*floor(wsi_r/patch_size)-4;
c_range = 2*floor(wsi_c/patch_size)-4;
normal_patches = {};

count =0;
while(length(normal_patches)< n_patches)
    r_Ind =  randi([2, r_range]);
    c_Ind =  randi([2, c_range]);
    count = count +1;
    if (count== 50000)
        return;
    end
    
    stop=0;
    if (ismember(r_Ind, r_list) ~= 0)
        [m,n] = find(r_Ind ==  r_list);
        % Put a for loop to avoid 
        % repeated patches 
        % could be multiple rows all need to
        % check separately
        for i=1:length(m)
            if (c_list(m(i)) == c_Ind)
                stop=1;
            end
        end
        if (stop==1)
            continue;
            stop=0;
        end
    end
    
    
    % overlapping
    r_Ind = r_Ind/2;
    c_Ind = c_Ind/2;
    r =  (r_Ind*patch_size)+1 ;
    c =  (c_Ind*patch_size)+1;
    
    
%     patch =  imread(wsiName, 'ReductionLevel', res_lev, 'PixelRegion', {[r-(patch_size/2)+1, r+(patch_size/2)],[c-(patch_size/2)+1, c+(patch_size/2)]});
    patch = openslide_read_region(wsiPtr,c,r,patch_size,patch_size ,res_lev);
    patch = patch(:,:,2:4);
    mask_patch = mask(r:r+patch_size-1, c:c+patch_size-1, 1);
    
    if (mean(patch(:)) <= 220 && mean(mask_patch(:)) == 0 )
        normal_patches = {normal_patches{:}, patch};
        % wsi_name, x, y, class, level, size 
        fprintf(fid,'%s,%d,%d,%d,%d,%d\r\n',wsiName, c, r, 1,res_lev,patch_size);
        r_list = [r_list; r_Ind*2];
        c_list = [c_list; c_Ind*2];

        if (mod(n_patches, length(normal_patches)) == 0 && length(normal_patches) > 100)
           length(normal_patches)
           count
        end
    end
    
end

end