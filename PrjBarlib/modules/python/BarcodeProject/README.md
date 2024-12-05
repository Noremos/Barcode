# A library to process raster images using persistence homology


Example:

	import raster_barcode as bc
	import raster_barcode.barcode as bcc
	import cv2

	img = cv2.imread('/Users/sam/Edu/bar/12/1.png', cv2.IMREAD_GRAYSCALE)

	cont = bc.barstruct()
	barc =  bcc.create_barcode(img, cont)

	cmp = barc.get_largest_component()
	img = bcc.combine_components_into_matrix(cmp, img.shape, img.dtype)
	cv2.imshow("source",img)
	cv2.waitKey(0)

	exit(0)
