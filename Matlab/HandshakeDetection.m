%import acceleromter data
fileName = 'Users/test/Documents/MATLAB/AccelerometerDataODS.ods';
array = zeros;
array = loadods2(fileName,2);
accelData = cell2mat(array)
%window loop
window = zeros(10,3);
wStart = 1;
wEnd = 10;
numHandshake = zeros;
index = 1;
while (wEnd <= length(accelData))
    window(:,1) = accelData(wStart:wEnd,1)
    window(:,2) = accelData(wStart:wEnd,2)
    window(:,3) = accelData(wStart:wEnd,3)
    x = mean(window);
    window(:,1) = window(:,1) - x(1);
    window(:,2) = window(:,2) - x(2);
    window(:,3) = window(:,3) - x(3);
    windowDFT = fft(window);
    mX = abs(windowDFT(:,1));
    mY = abs(windowDFT(:,2));
    %mZ = abs(windowDFT(:,3));
    [maxX,i] = max(mX);
    [maxY,j] = max(mY);
    %[maxZ,k] = max(mZ);
    z = mean(window(:,3));
    peakFX = (10/length(window))*(i - 1);
    rmsX = sqrt(sum(mX.^2)/length(mX));
    crX = maxX/rmsX;
    peakFY = (10/length(window))*(j - 1);
    rmsY = sqrt(sum(mY.^2)/length(mY));
    crY = maxY/rmsY;
    
    if (2 < peakFX && peakFX < 4.5 && 1.4 < crX && crX < 2.5 && 2.5 < peakFY && peakFY < 4.5 && 1.2 < crY && crY < 1.8 && z < 0.5 && z > -0.5)
         numHandshake(index) = 1;
         index = index + 1;
    else
         numHandshake(index) = 0;
         index = index + 1;
    end
    wStart = wStart + 1;
    wEnd = wEnd + 1;
end
numHandshake

%f = (0:length(accelData)-1)*10/length(accelData);
%plot(f,mZ); title('Magnitude');