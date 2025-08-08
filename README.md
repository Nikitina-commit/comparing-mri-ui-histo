# comparing_MRI_UI_histo
This is the interface for medical workers, allowing correct comparison of prostate gland sections obtained from different sources (MRI, ultrasound, histology)

System requirements:
Windows 10 (x64) or higher.
MinGW x64

How to build it:
1. Please install Qt Creator 5.0.0 or higher.
2. Download all files in one path.
3. From IDE open "*.pro" file like project.

The input data: medical results of ultrasound, MRI, and histology studies. Histological sections were obtained using an optical microscope in PNG format, ultrasound and MRI images in DICOM or PNG formats. To conduct the experiment, it is necessary to obtain several dozen slices from each source, obtained from one patient.

Output data: 
1. Tables of pixel locations: "output_board.csv", "output_tumor.csv". 
2. A series of images of overlapping contours of the gland and neoplasm from various sources. The number of images in a series is equal to the number of images in a series of histological data.

The principle of operation:
1. Click "Upload", select all the slice images in the correct order one by one.
2. Select an image from the list, and you can zoom in on it.
3. Set the slice thickness in pixels.
4. Click the "Crop" button, select the area with the mouse, and then click the button again. 
5. Rotate the image if necessary. 
6. Click the "board" button, then circle the outline of the gland with the mouse. If you are not satisfied with the outline,
then click the button again and try again. 
7. Click the "tumor" button, then circle the outline of the gland with the mouse. 
8. Click the "Save boards" button. 
9. Press the "axes" button, the axes will appear in the center of mass of the gland, you can move them. 
10. Click the "Save position" button.
11. Repeat steps 2-8 for all images in the sequence. 
12. Press one of the "Save as ..." buttons
13. Repeat steps 1-12 for all three research methods.
14. Click the "Save as" button..."one more time. 
After pressing the save button, it will light up blue on the screen and you will see slices in y/z coordinates.
15. To calculate the histological model, simply click "Calculate". 
16. For ultrasound or MRI models, find
the presumed bottom of the gland, a flat area 
17. Connect the extreme points of the flattening with a line. 
18. Click the "Calculate" button. 
19. Select the required model for comparison with histological sections. 
20. Click the "Compare" button
