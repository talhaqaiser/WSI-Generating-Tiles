function mask = maskShrinkBounderies(mask, flag)
    
    mask = im2double(mask(:,:,1)); 
    mask = im2bw(mask);
    SE = strel('disk',4);
%     SE = strel('disk',1);
    if (flag == 't')
        er_mask = imerode(mask, SE);
    else
        er_mask = imdilate(mask, SE);
    end
    
    mask =  (mask + er_mask)/2;
end