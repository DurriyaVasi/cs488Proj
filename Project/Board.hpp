class Board {
	public: 

	float lowXBoundary;
	float lowYBoundary;
	float highXBoundary;
	float highYBoundary;
	
	Board(float lowXBoundary, float lowYBoundary, float highXBoundary, float highYBoundary)
		: lowXBoundary(lowXBoundary),
		  lowYBoundary(lowYBoundary),
		  highXBoundary(highXBoundary),
		  highYBoundary(highYBoundary) {}
}
		
