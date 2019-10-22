int GetMyBuddyToRank(int numRows, int numCols, int row, int col)
{
    return row * numCols + col;
}
int GetMyBuddyFromRank(int numRows, int numCols, int row, int col)
{
    int curr_rank = row * numRows + col;
    printf("-%d-", curr_rank);
    return curr_rank / numCols + curr_rank % numCols;
}
