#include <bits/stdc++.h>

using namespace std;
class MatrixMultiplication{
    vector<vector<int> > matrix1, matrix2, multiply;
    struct arguments{
        vector<vector<int> > *matrix1, *matrix2, *multiply;
        int row1;
        int col2;
        arguments(vector<vector<int> > &matrix1, vector<vector<int> > &matrix2, vector<vector<int> > &multiply, int row1, int col2) : matrix1(&matrix1), matrix2(&matrix2), multiply(&multiply), row1(row1), col2(col2) {}
    };
public:
    MatrixMultiplication(vector<vector<int> > &matrix1, vector<vector<int> > &matrix2){
        this -> matrix1 = matrix1;
        this -> matrix2 = matrix2;
        multiply.resize(matrix1.size());
    }
    vector<vector<int> > executeVariation1(){
        if(!check()){
            return multiply;
        }
        int th = matrix1.size()*matrix2[0].size();
        pthread_t thread[th];
        int t = 0;
        for (int r1 = 0; r1 < matrix1.size(); r1++){
            multiply[r1].resize(matrix2[0].size());
            for (int c2 = 0; c2 < matrix2[0].size(); c2++){
                arguments *args = new arguments(matrix1, matrix2, multiply, r1, c2);
                int rc = pthread_create(&thread[t++], NULL, computeElement, (void *) args);
                if(rc){
                    cerr << "Error:unable to create thread," << rc << endl;
                    exit(-1);
                }
            }
        }
        for(t = 0; t < th; t++){
            int err = pthread_join(thread[t], NULL); // suspends execution of the calling thread until the target thread terminates
            if(err != 0){
                cerr << err << endl;
            }
        }
        return multiply;
    }
    vector<vector<int> > executeVariation2(){
        if(!check()){
            return multiply;
        }
        pthread_t thread[matrix1.size()];
        for (int r1 = 0; r1 < matrix1.size(); r1++){
            multiply[r1].resize(matrix2[0].size());
            arguments *args = new arguments(matrix1, matrix2, multiply, r1, 0);
            int rc = pthread_create(&thread[r1], NULL, computeRow, (void *) args);
            if(rc){
                cerr << "Error:unable to create thread," << rc << endl;
                exit(-1);
            }
        }
        for(int r1 = 0; r1 < matrix1.size(); r1++){
            int err = pthread_join(thread[r1], NULL); // suspends execution of the calling thread until the target thread terminates
            if(err != 0){
                cerr << err << endl;
            }
        }
        return multiply;
    }
    void out(string filePath, bool append){
        ofstream out(filePath, append ? std::ios_base::app : std::ios_base::out);
        for(int i=0; i<multiply.size(); i++){
            for(int j=0; j<multiply[i].size(); j++){
                out << multiply[i][j] << ' ';
            }
            out << endl;
        }
        return;
    }
private:
    static void* computeElement(void* args){
        arguments* curr_args = (arguments*) args;
        vector<vector<int> > *matrix1 = curr_args->matrix1;
        vector<vector<int> > *matrix2 = curr_args->matrix2;
        vector<vector<int> > *multiply = curr_args->multiply;
        int r1 = curr_args->row1;
        int c2 = curr_args->col2;
        int sum = 0;
        for (int i = 0; i < (*matrix2).size(); i++){
            sum += (*matrix1)[r1][i] * (*matrix2)[i][c2];
        }
        (*multiply)[r1][c2] = sum;
        return NULL;
    }
    static void* computeRow(void* args){
        arguments* curr_args = (arguments*) args;
        vector<vector<int> > *matrix1 = curr_args->matrix1;
        vector<vector<int> > *matrix2 = curr_args->matrix2;
        vector<vector<int> > *multiply = curr_args->multiply;
        int r1 = curr_args->row1;
        for (int c2 = 0; c2 < matrix2[0].size(); c2++){
            int sum = 0;
            for (int i = 0; i < (*matrix2).size(); i++){
                sum += (*matrix1)[r1][i] * (*matrix2)[i][c2];
            }
            (*multiply)[r1][c2] = sum;
        }
        return NULL;
    }
    // checks if matrices can be multiplied
    bool check(){
        if(matrix1.empty() || matrix2.empty()){
            cerr << "One or both matrices are empty.";
            return false;
        }
        if (matrix1[0].size() != matrix2.size()){
            cerr << "The matrices can't be multiplied.\n";
            return false;
        }
        return true;
    }
};
int main()
{
    ifstream in("100.txt");
    int r1, c1;
    in >> r1 >> c1;
    vector<vector<int> > matrix1(r1, vector<int>(c1));
    for(int i=0; i<r1; i++){
        for(int j=0; j<c1; j++){
            in >> matrix1[i][j];
        }
    }
    int r2, c2;
    in >> r2 >> c2;
    vector<vector<int> > matrix2(r2, vector<int>(c2));
    for(int i=0; i<r2; i++){
        for(int j=0; j<c2; j++){
            in >> matrix2[i][j];
        }
    }
    clock_t t1, t2;
    MatrixMultiplication matrixMult(matrix1, matrix2);

    t1 = clock();
    matrixMult.executeVariation1();
    t2 = clock();
    matrixMult.out("output.txt", false);
    ofstream out("output.txt", std::ios_base::app);
    out << "END1 " << ((t2 - t1) / (double)CLOCKS_PER_SEC) << endl;

    t1 = clock();
    matrixMult.executeVariation2();
    t2 = clock();
    matrixMult.out("output.txt", true);
    out << "END2 " << ((t2 - t1) / (double)CLOCKS_PER_SEC) << endl;

    return 0;
}
