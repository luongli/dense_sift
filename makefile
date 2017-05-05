CC = g++
LDFLAGS = -ggdb -L/usr/local/lib -lopencv_stitching -lopencv_superres \
			-lopencv_videostab -lopencv_aruco -lopencv_bgsegm -lopencv_bioinspired \
			-lopencv_ccalib -lopencv_dpm -lopencv_freetype -lopencv_fuzzy -lopencv_hdf \
			-lopencv_line_descriptor -lopencv_optflow -lopencv_reg -lopencv_saliency \
			-lopencv_stereo -lopencv_structured_light -lopencv_phase_unwrapping \
			-lopencv_rgbd -lopencv_surface_matching -lopencv_tracking -lopencv_datasets \
			-lopencv_text -lopencv_face -lopencv_plot -lopencv_dnn -lopencv_xfeatures2d \
			-lopencv_shape -lopencv_video -lopencv_ximgproc -lopencv_calib3d -lopencv_features2d \
			-lopencv_flann -lopencv_xobjdetect -lopencv_objdetect -lopencv_ml -lopencv_xphoto \
			-lopencv_highgui -lopencv_videoio -lopencv_imgcodecs -lopencv_photo -lopencv_imgproc \
			-lopencv_core
CFLAGS = -I/usr/local/include/opencv -I/usr/local/include
all: dense_sift
dense_sift: 
	$(CC) -g -o dense_sift dense_sift.cpp $(LDFLAGS) $(CFLAGS)
clean:
	rm dense_sift
