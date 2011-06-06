#include <irtkImage.h>

#include <irtkRegistration.h>

#include <irtkGaussianBlurring.h>

#include <irtkGaussianBlurring4D.h>

char *dofout_name = NULL, *parin_name  = NULL, *parout_name = NULL, **filenames = NULL, *mask_name = NULL;

void usage()
{
	cerr << "Usage: motiontrackmultimage [image sequence] <options> \n" << endl;
	cerr << "Registration using all image sequences at the same time \n" << endl;
	cerr << "where <options> is one or more of the following:\numberOfImages" << endl;
	cerr << "<-parin file>        Read parameter from file" << endl;
	cerr << "<-parout file>       Write parameter to file" << endl;
	cerr << "<-dofout file>       Write transformation to file" << endl;
	cerr << "<-ref file>          Reference time frame (default = first frame of image sequence)" << endl;
	cerr << "<-Rx1 value>         Region of interest in images" << endl;
	cerr << "<-Ry1 value>         Region of interest in images" << endl;
	cerr << "<-Rz1 value>         Region of interest in images" << endl;
	cerr << "<-Rt1 value>         Region of interest in images" << endl;
	cerr << "<-Rx2 value>         Region of interest in images" << endl;
	cerr << "<-Ry2 value>         Region of interest in images" << endl;
	cerr << "<-Rz2 value>         Region of interest in images" << endl;
	cerr << "<-Rt2 value>         Region of interest in images" << endl;
	cerr << "<-Tp  value>         Padding value" << endl;
	cerr << "<-mask file>         Use a mask to define the ROI. The mask" << endl;
	cerr << "<-landmarks name>    Landmark Regulation input name is prefix" << endl;
    cerr << "<-weighting levelnumber weight1...weightn>    weighting for the images" << endl;
    exit(1);
}


int main(int argc, char **argv)
{
	int l, i, j, numberOfImages, t, x, y, z, x1, y1, z1, t1, x2, y2, z2, t2, ok, debug;
	double spacing, sigma, xaxis[3], yaxis[3], zaxis[3],**weight;
	irtkGreyPixel padding;
	irtkMultiLevelFreeFormTransformation *mffd;
	irtkPointSet **landmarks;

	// Check command line
	if (argc < 2) {
		usage();
	}

	// Get image names for sequence
	numberOfImages = 0;
	filenames = argv;
    weight = new double*[10];
	while ((argc > 1) && (argv[1][0] != '-' )) {
		argv++;
		argc--;
		numberOfImages++;
	}
	filenames++;
	landmarks = NULL;
    for (i = 0; i < 10; i++){
        weight[i] = new double[numberOfImages];
        for (j = 0; j < numberOfImages; j++) {
            weight[i][j] = 1;
        }
    }

	// Read image sequence
	cout << "Reading image sequence ... "; cout.flush();
	irtkGreyImage **image = new irtkGreyImage *[numberOfImages];
	for (i = 0; i < numberOfImages; i++) {
		cout << filenames[i] << endl;
		image[i] = new irtkGreyImage(filenames[i]);
	}

	// Check if there is at least one image
	if (numberOfImages == 0) {
		usage();
	}

	// Fix ROI
	x1 = 0;
	y1 = 0;
	z1 = 0;
	t1 = 0;
	x2 = image[0]->GetX();
	y2 = image[0]->GetY();
	z2 = image[0]->GetZ();
	t2 = image[0]->GetT();

	// Default parameters
	padding   = MIN_GREY;
	spacing   = 0;
	sigma     = 0;
	debug     = false;

	// Parse remaining parameters
	while (argc > 1) {
		ok = false;
		if ((ok == false) && (strcmp(argv[1], "-Rx1") == 0)) {
			argc--;
			argv++;
			x1 = atoi(argv[1]);
			argc--;
			argv++;
			ok = true;
		}
		if ((ok == false) && (strcmp(argv[1], "-Rx2") == 0)) {
			argc--;
			argv++;
			x2 = atoi(argv[1]);
			argc--;
			argv++;
			ok = true;
		}
		if ((ok == false) && (strcmp(argv[1], "-Ry1") == 0)) {
			argc--;
			argv++;
			y1 = atoi(argv[1]);
			argc--;
			argv++;
			ok = true;
		}
		if ((ok == false) && (strcmp(argv[1], "-Ry2") == 0)) {
			argc--;
			argv++;
			y2 = atoi(argv[1]);
			argc--;
			argv++;
			ok = true;
		}
		if ((ok == false) && (strcmp(argv[1], "-Rz1") == 0)) {
			argc--;
			argv++;
			z1 = atoi(argv[1]);
			argc--;
			argv++;
			ok = true;
		}
		if ((ok == false) && (strcmp(argv[1], "-Rz2") == 0)) {
			argc--;
			argv++;
			z2 = atoi(argv[1]);
			argc--;
			argv++;
			ok = true;
		}
		if ((ok == false) && (strcmp(argv[1], "-Rt1") == 0)) {
			argc--;
			argv++;
			t1 = atoi(argv[1]);
			argc--;
			argv++;
			ok = true;
		}
		if ((ok == false) && (strcmp(argv[1], "-Rt2") == 0)) {
			argc--;
			argv++;
			t2 = atoi(argv[1]);
			argc--;
			argv++;
			ok = true;
		}
		if ((ok == false) && (strcmp(argv[1], "-dofout") == 0)) {
			argc--;
			argv++;
			dofout_name = argv[1];
			argc--;
			argv++;
			ok = true;
		}

		if ((ok == false) && (strcmp(argv[1], "-Tp") == 0)) {
			argc--;
			argv++;
			padding = atoi(argv[1]);
			argc--;
			argv++;
			ok = true;
		}

		if ((ok == false) && (strcmp(argv[1], "-ds") == 0)) {
			argc--;
			argv++;
			spacing = atof(argv[1]);
			argc--;
			argv++;
			ok = true;
		}
		if ((ok == false) && (strcmp(argv[1], "-parin") == 0)) {
			argc--;
			argv++;
			ok = true;
			parin_name = argv[1];
			argc--;
			argv++;
		}
		if ((ok == false) && (strcmp(argv[1], "-parout") == 0)) {
			argc--;
			argv++;
			ok = true;
			parout_name = argv[1];
			argc--;
			argv++;
		}
		if ((ok == false) && (strcmp(argv[1], "-blur") == 0)) {
			argc--;
			argv++;
			ok = true;
			sigma = atof(argv[1]);
			argc--;
			argv++;
		}
		if ((ok == false) && (strcmp(argv[1], "-debug") == 0)) {
			argc--;
			argv++;
			ok = true;
			debug = true;
		}
		if ((ok == false) && (strcmp(argv[1], "-mask") == 0)) {
			argc--;
			argv++;
			mask_name = argv[1];
			argc--;
			argv++;
			ok = true;
		}
		if ((ok == false) && (strcmp(argv[1], "-weighting") == 0)) {
			argc--;
			argv++;
            int level = atoi(argv[1]);
            if(level > 9){
               cout << "Number of level < 10" << endl;
               exit(1);
            }
            argc--;
            argv++;
			cout << "Setting weighting for level: " << level << endl; cout.flush();
			for (i = 0; i < numberOfImages; i++) {
                weight[level][i] = atof(argv[1]);
                argc--;
                argv++;
			}
			ok = true;
		}
        if ((ok == false) && (strcmp(argv[1], "-landmarks") == 0)) {
            argc--;
            argv++;
            cout << "Reading landmark sequence ... "; cout.flush();
            landmarks = new irtkPointSet *[image[0]->GetT()];
            for (i = 0; i < image[0]->GetT(); i++) {
                char buffer[255];
                sprintf(buffer, "%s%.2d.vtk", argv[1],i);
                cout << buffer << endl;
                landmarks[i] = new irtkPointSet();
                landmarks[i]->ReadVTK(buffer);
            }
            argc--;
            argv++;
            ok = true;
        }
		if (ok == false) {
			cerr << "Can not parse argument " << argv[1] << endl;
			usage();
		}
	}

	// Image orientation
	image[0]->GetOrientation(xaxis, yaxis, zaxis);

	// If there is an region of interest, use it
	if ((x1 != 0) || (x2 != image[0]->GetX()) ||
		(y1 != 0) || (y2 != image[0]->GetY()) ||
		(z1 != 0) || (z2 != image[0]->GetZ()) ||
		(t1 != 0) || (t2 != image[0]->GetT())) {
			for (i = 0; i < numberOfImages; i++) {
				*image[i] = image[i]->GetRegion(x1, y1, z1, t1, x2, y2, z2, t2);
			}
	}

	// If sigma is larger than 0, blur images using 4D blurring
	if (sigma > 0) {
		cout << "Blurring image sequences ... "; cout.flush();
		for (i = 0; i < numberOfImages; i++) {
			irtkGaussianBlurring4D<irtkGreyPixel> gaussianBlurring4D(sigma);
			gaussianBlurring4D.SetInput (image[i]);
			gaussianBlurring4D.SetOutput(image[i]);
			gaussianBlurring4D.Run();
		}
		cout << "done" << endl;
	}

	// Use identity transformation to start
	mffd = new irtkMultiLevelFreeFormTransformation;
	for (t = 1; t < image[0]->GetT(); t++) {
		// Create registration filter
		irtkMultipleImageFreeFormRegistration *multimageregistration = NULL;
		if (image[0]->GetZ() == 1) {
			cerr<<"this mode can't be used with 2D images"<<endl;
			exit(1);
		} else {
			multimageregistration = new irtkMultipleImageFreeFormRegistration;
		}

		// Combine images
		irtkGreyImage **target = new irtkGreyImage*[numberOfImages];
		irtkGreyImage **source = new irtkGreyImage*[numberOfImages];
		for(l=0;l<numberOfImages;l++){
			irtkImageAttributes attr = image[l]->GetImageAttributes();
			attr._t = 1;
			target[l] = new irtkGreyImage(attr);
			source[l] = new irtkGreyImage(attr);
			for (z = 0; z < target[l]->GetZ(); z++) {
				for (y = 0; y < target[l]->GetY(); y++) {
					for (x = 0; x < target[l]->GetX(); x++) {
						target[l]->Put(x, y, z, 0, image[l]->Get(x, y, z, 0));
						source[l]->Put(x, y, z, 0, image[l]->Get(x, y, z, t));
					}
				}
			}
		}

		if (mask_name != NULL) {
			irtkGreyImage mask(mask_name);
			irtkInterpolateImageFunction *interpolator = new irtkShapeBasedInterpolateImageFunction;
			double xsize, ysize, zsize, size;
			mask.GetPixelSize(&xsize, &ysize, &zsize);
			size = xsize;
			size = (size < ysize) ? size : ysize;
			size = (size < zsize) ? size : zsize;
			irtkResampling<irtkGreyPixel> resampling(size, size, size);
			resampling.SetInput (&mask);
			resampling.SetOutput(&mask);
			resampling.SetInterpolator(interpolator);
			resampling.Run();
			delete interpolator;
			for(l=0;l<numberOfImages;l++){
				for (z = 0; z < target[l]->GetZ(); z++) {
					for (y = 0; y < target[l]->GetY(); y++) {
						for (x = 0; x < target[l]->GetX(); x++) {
							double wx,wy,wz;
							wx = x; wy = y; wz = z;
							target[l]->ImageToWorld(wx,wy,wz);
							mask.WorldToImage(wx,wy,wz);
							wx = round(wx); wy = round(wy); wz = round(wz);
							if(wx >= 0 && wx < mask.GetX() && wy >= 0 && wy < mask.GetY()
								&& wz >= 0 && wz < mask.GetZ() &&mask.GetAsDouble(wx,wy,wz) <= 0){
								target[l]->Put(x, y, z, 0, padding);
							}
						}
					}
				}
			}
		}

		// Set input and output for the registration filter
		multimageregistration->SetInput(target, source, numberOfImages);
		if(landmarks != NULL){
			multimageregistration->SetLandmarks(landmarks[0],landmarks[t]);
		}
		multimageregistration->SetOutput(mffd);
		multimageregistration->SetDebugFlag(debug);

		// Read parameter if there any, otherwise make an intelligent guess
		if (parin_name != NULL) {
			multimageregistration->irtkMultipleImageRegistration::Read(parin_name);
		} else {
			multimageregistration->GuessParameter();
		}

		// Override parameter settings if necessary
		if (padding != MIN_GREY) {
			multimageregistration->SetTargetPadding(padding);
		}
		if (spacing > 0) {
			multimageregistration->SetDX(spacing);
			multimageregistration->SetDY(spacing);
			multimageregistration->SetDZ(spacing);
		}

		// Write parameters if necessary
		if (parout_name != NULL) {
			multimageregistration->irtkMultipleImageRegistration::Write(parout_name);
		}

        multimageregistration->SetWeighting(weight);

		// Run registration filter
		multimageregistration->Run();

		if(multimageregistration->GetMFFDMode()){
			mffd->CombineLocalTransformation();
		}

		// Write the final transformation estimate
		if (dofout_name != NULL) {
			char buffer[255];
			sprintf(buffer, "%s%d_sequence_%.2d.dof.gz", dofout_name,i, t);
			mffd->irtkTransformation::Write(buffer);
		}

		delete multimageregistration;

		for (l = 0; l < numberOfImages; l++) {
			delete target[l];
			delete source[l];
		}

		delete []target;
		delete []source;
	}

    for(i = 0; i < 10; i++){
        delete []weight[i];
    }
    delete []weight;
}
