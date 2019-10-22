int GetMyBuddyToRank(int numRows, int numCols, int row, int col)
{
    int curr_rank = col * numRows + row;
    int rrow, rcol;
    rrow = curr_rank / numCols;
    rcol = curr_rank % numCols;
    return row * numCols + col;
}
int GetMyBuddyFromRank(int numRows, int numCols, int row, int col)
{
    int curr_rank = col * numRows + row;
    return row * numCols + col;
}
