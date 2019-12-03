
#define XY_TO_INDEX(X,Y) (((Y) * lineWidth) + ( X ))
__constant bool lookup[2][9] = {
		{0,0,0,1,0,0,0,0,0},
		{0,0,1,1,0,0,0,0,0}
	};

__kernel void gol_Step(__global const bool *in, __global bool *out, int lineWidth, int lineHeight) {

	//printf("Lookup");
 
    // Get the index of the current element to be processed
    int index = get_global_id(0);

	int y = index / lineWidth;
	int x = index - (y * lineWidth);

	int line1 = ((y - 1) < 0) ? lineHeight - 1 : (y - 1);
	int line3 = ((y + 1) >= lineHeight) ? 0 : (y + 1);

	int row1 = ((x - 1) < 0) ? lineWidth - 1 : (x - 1);
	int row3 = ((x + 1) >= lineWidth) ? 0 : (x + 1);

	int nrOfN = in[XY_TO_INDEX(row1, line1)]
		+ in[XY_TO_INDEX(x, line1)]
		+ in[XY_TO_INDEX(row3, line1)]
		+ in[XY_TO_INDEX(row1, y)]
		+ in[XY_TO_INDEX(row3, y)]
		+ in[XY_TO_INDEX(row1, line3)]
		+ in[XY_TO_INDEX(x, line3)]
		+ in[XY_TO_INDEX(row3, line3)];

	//printf("Neighbors: %d",nrOfN);

	out[index] = lookup[in[index]][nrOfN];
}