#include <gtest/gtest.h>
#include "data_structures/Matrix.hxx"
#include "utils/mathUtilities.hxx"

class MatrixTesting : public ::testing::Test {
 protected:
  void SetUp() override {
    // q0_ remains empty
    m0_  = new Matrix<double>();
    m1_ = new Matrix<double>(5, 10);
    double** tmp = new double*[10];
    for(int i = 0;i < 10; i++){
      tmp[i] = new double[12];
    }
    for(int i = 0;i < 10; i++){
      for(int j = 0;j < 12; j++){
        tmp[i][j] = 100;
      }
    }
    m2_ = new Matrix<double>(tmp, 10, 12);
    m3_ = new Matrix<double>(*m2_);

     
  }
  void TearDown() override{
    delete m0_;
    delete m1_;
    delete m2_;
    delete m3_;
  }

  // void TearDown() override {}

  Matrix<double>* m0_;       //testing default constructor
  Matrix<double>* m1_;    //testing rows and cols constructor
  Matrix<double>* m2_;  //testing passing another matrix as array input to constructor
  Matrix<double>* m3_;  //testing passing another matrix as input to constructor (= operator)
};

TEST_F(MatrixTesting, constructorWorksDefault) {
  EXPECT_EQ(m0_->getCols(), 1);
  EXPECT_EQ(m0_->getRows(), 1);
  EXPECT_FLOAT_EQ(m0_->getValue(0,0), 0);
}


TEST_F(MatrixTesting, constructorWorksRowsAndCols) {
  EXPECT_EQ(m1_->getCols(), 10);
  EXPECT_EQ(m1_->getRows(), 5);
  for (int i = 0; i<m1_->getRows(); i++) {
    for (int j = 0; j<m1_->getCols(); j++) {
      EXPECT_FLOAT_EQ(m1_->getValue(i,j), 0);
    }
  }
}

TEST_F(MatrixTesting, constructorWorksPassingArray) {
  EXPECT_EQ(m2_->getCols(), 12);
  EXPECT_EQ(m2_->getRows(), 10);
  for (int i = 0; i<m2_->getRows(); i++) {
    for (int j = 0; j<m2_->getCols(); j++) {
      EXPECT_FLOAT_EQ(m2_->getValue(i,j), 100);
    }
  }
}

TEST_F(MatrixTesting, constructorWorksPassingMatrix) {
  EXPECT_EQ(m3_->getCols(), 12);
  EXPECT_EQ(m3_->getRows(), 10);
  for (int i = 0; i<m3_->getRows(); i++) {
    for (int j = 0; j<m3_->getCols(); j++) {
      EXPECT_FLOAT_EQ(m3_->getValue(i,j), m2_->getValue(i,j) );
    }
  }
}

TEST_F(MatrixTesting,multiplicationControlDimensions){
  Matrix<double> matrixres = (*m1_) * (*m2_);
  EXPECT_EQ(matrixres.getRows(), 5);
  EXPECT_EQ(matrixres.getCols(), 12);
}

TEST_F(MatrixTesting,multiplicationControlResults){
  Matrix<double> matrixres = (*m1_) * (*m2_);
  for (int i=0;i<matrixres.getRows() ;i++) {
      for (int j=0;j<matrixres.getCols() ;j++) {
          EXPECT_FLOAT_EQ( matrixres.getValue(i,j),0);
      }
  }
}

TEST_F(MatrixTesting,assignmentWorksMatrix){
  Matrix<double> matrixres = *m2_;
  EXPECT_EQ(matrixres.getCols(), 12);
  EXPECT_EQ(matrixres.getRows(), 10);
  for (int i = 0; i<m2_->getRows(); i++) {
    for (int j = 0; j<matrixres.getCols(); j++) {
      EXPECT_FLOAT_EQ(matrixres.getValue(i,j), 100);
    }
  }
}

// TODO test all the functionalities of matrix, especially matrix multiplication, addition and copy with addition of rows


TEST_F(MatrixTesting,conversionToArmadilloMat){
  Matrix<double> matrixres = *m2_;
  matrixres(2,3)=4.0;
  matrixres(4,3)=5.0;
  arma::Mat<double> test = matrixres.asArmadilloMatrix();
  EXPECT_EQ(test.n_cols, 12);
  EXPECT_EQ(test.n_rows, 10);
  for (int i = 0; i<SizeToInt( test.n_rows); i++) {
    for (int j = 0; j<SizeToInt(test.n_rows); j++) {
      if(i==2 && j==3) EXPECT_FLOAT_EQ(test(i,j), 4.0);
      else if(i==4 && j==3) EXPECT_FLOAT_EQ(test(i,j), 5.0);
      else EXPECT_FLOAT_EQ(test(i,j), 100);
    }
  }
}

TEST_F(MatrixTesting,testAddColNewStart){
  std::vector<double> row = {1,2,3,4,5,6,7,8,9,10,11,12};
  Matrix<double>* matrixres = m2_->addRowNew(row,0);
  EXPECT_EQ(matrixres->getCols(), 12);
  EXPECT_EQ(matrixres->getRows(), 11);
  for (int i = 0; i<matrixres->getRows(); i++) {
    for (int j = 0; j<matrixres->getCols(); j++) {
      if(i==0) EXPECT_FLOAT_EQ(matrixres->getValue(i,j), row[j]);
      else EXPECT_FLOAT_EQ(matrixres->getValue(i,j), 100);
    }
  }
  delete matrixres;
}

TEST_F(MatrixTesting,testAddColNewMiddle){
  std::vector<double> row = {1,2,3,4,5,6,7,8,9,10,11,12};
  Matrix<double>* matrixres = m2_->addRowNew(row,5);
  EXPECT_EQ(matrixres->getCols(), 12);
  EXPECT_EQ(matrixres->getRows(), 11);
  for (int i = 0; i<matrixres->getRows(); i++) {
    for (int j = 0; j<matrixres->getCols(); j++) {
      if(i==5) EXPECT_FLOAT_EQ(matrixres->getValue(i,j), row[j]);
      else EXPECT_FLOAT_EQ(matrixres->getValue(i,j), 100);
    }
  }
  delete matrixres;
}

TEST_F(MatrixTesting,testAddColNewEnd){
  std::vector<double> row = {1,2,3,4,5,6,7,8,9,10,11,12};
  Matrix<double>* matrixres = m2_->addRowNew(row,10);
  EXPECT_EQ(matrixres->getCols(), 12);
  EXPECT_EQ(matrixres->getRows(), 11);
  for (int i = 0; i<matrixres->getRows(); i++) {
    for (int j = 0; j<matrixres->getCols(); j++) {
      if(i==10) EXPECT_FLOAT_EQ(matrixres->getValue(i,j), row[j]);
      else EXPECT_FLOAT_EQ(matrixres->getValue(i,j), 100);
    }
  }
  delete matrixres;
}

TEST_F(MatrixTesting,testAddRowNewStart){
  std::vector<double> row = {1,2,3,4,5,6,7,8,9,10,11,12};
  Matrix<double>* matrixres = m2_->addRowNew(row,0);
  EXPECT_EQ(matrixres->getCols(), 12);
  EXPECT_EQ(matrixres->getRows(), 11);
  for (int i = 0; i<matrixres->getRows(); i++) {
    for (int j = 0; j<matrixres->getCols(); j++) {
      if(i==0) EXPECT_FLOAT_EQ(matrixres->getValue(i,j), row[j]);
      else EXPECT_FLOAT_EQ(matrixres->getValue(i,j), 100);
    }
  }
  delete matrixres;
}

TEST_F(MatrixTesting,testAddRowNewMiddle){
  std::vector<double> row = {1,2,3,4,5,6,7,8,9,10,11,12};
  Matrix<double>* matrixres = m2_->addRowNew(row,5);
  EXPECT_EQ(matrixres->getCols(), 12);
  EXPECT_EQ(matrixres->getRows(), 11);
  for (int i = 0; i<matrixres->getRows(); i++) {
    for (int j = 0; j<matrixres->getCols(); j++) {
      if(i==5) EXPECT_FLOAT_EQ(matrixres->getValue(i,j), row[j]);
      else EXPECT_FLOAT_EQ(matrixres->getValue(i,j), 100);
    }
  }
  delete matrixres;
}

TEST_F(MatrixTesting,testAddRowNewEnd){
  std::vector<double> row = {1,2,3,4,5,6,7,8,9,10,11,12};
  Matrix<double>* matrixres = m2_->addRowNew(row,10);
  EXPECT_EQ(matrixres->getCols(), 12);
  EXPECT_EQ(matrixres->getRows(), 11);
  for (int i = 0; i<matrixres->getRows(); i++) {
    for (int j = 0; j<matrixres->getCols(); j++) {
      if(i==10) EXPECT_FLOAT_EQ(matrixres->getValue(i,j), row[j]);
      else EXPECT_FLOAT_EQ(matrixres->getValue(i,j), 100);
    }
  }
  delete matrixres;
}

TEST_F(MatrixTesting,testAddRowStart){
  std::vector<double> row = {1,2,3,4,5,6,7,8,9,10,11,12};
  Matrix<double> testing_matrix = *m2_;
  testing_matrix.addRow(row,0);
  EXPECT_EQ(testing_matrix.getCols(), 12);
  EXPECT_EQ(testing_matrix.getRows(), 11);
  for (int i = 0; i<testing_matrix.getRows(); i++) {
    for (int j = 0; j<testing_matrix.getCols(); j++) {
      if(i==0) EXPECT_FLOAT_EQ(testing_matrix.getValue(i,j), row[j]);
      else EXPECT_FLOAT_EQ(testing_matrix.getValue(i,j), 100);
    }
  }
}

TEST_F(MatrixTesting,testAddRowMiddle){
  std::vector<double> row = {1,2,3,4,5,6,7,8,9,10,11,12};
  Matrix<double> testing_matrix = *m2_;
  testing_matrix.addRow(row,5);
  EXPECT_EQ(testing_matrix.getCols(), 12);
  EXPECT_EQ(testing_matrix.getRows(), 11);
  for (int i = 0; i<testing_matrix.getRows(); i++) {
    for (int j = 0; j<testing_matrix.getCols(); j++) {
      if(i==5) EXPECT_FLOAT_EQ(testing_matrix.getValue(i,j), row[j]);
      else EXPECT_FLOAT_EQ(testing_matrix.getValue(i,j), 100);
    }
  }
}

TEST_F(MatrixTesting,testAddRowEnd){
  std::vector<double> row = {1,2,3,4,5,6,7,8,9,10,11,12};
  Matrix<double> testing_matrix = *m2_;
  testing_matrix.addRow(row,10);
  EXPECT_EQ(testing_matrix.getCols(), 12);
  EXPECT_EQ(testing_matrix.getRows(), 11);
  for (int i = 0; i<testing_matrix.getRows(); i++) {
    for (int j = 0; j<testing_matrix.getCols(); j++) {
      if(i==10) EXPECT_FLOAT_EQ(testing_matrix.getValue(i,j), row[j]);
      else EXPECT_FLOAT_EQ(testing_matrix.getValue(i,j), 100);
    }
  }
}


TEST_F(MatrixTesting,testAddColStart){
  std::vector<double> col = {1,2,3,4,5,6,7,8,9,10,11,12};
  Matrix<double> testing_matrix = *m2_;
  testing_matrix.addColumn(col,0);
  EXPECT_EQ(testing_matrix.getCols(), 13);
  EXPECT_EQ(testing_matrix.getRows(), 10);
  for (int i = 0; i<testing_matrix.getRows(); i++) {
    for (int j = 0; j<testing_matrix.getCols(); j++) {
      if(j==0) EXPECT_FLOAT_EQ(testing_matrix.getValue(i,j), col[i]);
      else EXPECT_FLOAT_EQ(testing_matrix.getValue(i,j), 100);
    }
  }
}

TEST_F(MatrixTesting,testAddColMiddle){
  std::vector<double> col = {1,2,3,4,5,6,7,8,9,10,11,12};
  Matrix<double> testing_matrix = *m2_;
  testing_matrix.addColumn(col,5);
  EXPECT_EQ(testing_matrix.getCols(), 13);
  EXPECT_EQ(testing_matrix.getRows(), 10);
  for (int i = 0; i<testing_matrix.getRows(); i++) {
    for (int j = 0; j<testing_matrix.getCols(); j++) {
      if(j==5) EXPECT_FLOAT_EQ(testing_matrix.getValue(i,j), col[i]);
      else EXPECT_FLOAT_EQ(testing_matrix.getValue(i,j), 100);
    }
  }
}

TEST_F(MatrixTesting,testAddColEnd){
  std::vector<double> col = {1,2,3,4,5,6,7,8,9,10,11,12};
  Matrix<double> testing_matrix = *m2_;
  testing_matrix.addColumn(col,12);
  EXPECT_EQ(testing_matrix.getCols(), 13);
  EXPECT_EQ(testing_matrix.getRows(), 10);
  for (int i = 0; i<testing_matrix.getRows(); i++) {
    for (int j = 0; j<testing_matrix.getCols(); j++) {
      if(j==12) EXPECT_FLOAT_EQ(testing_matrix.getValue(i,j), col[i]);
      else EXPECT_FLOAT_EQ(testing_matrix.getValue(i,j), 100);
    }
  }
}

TEST_F(MatrixTesting,testAddColEndQuick){
  std::vector<double> col = {1,2,3,4,5,6,7,8,9,10,11,12};
  Matrix<double> testing_matrix = *m2_;
  testing_matrix.addColumnAtTheEnd(col);
  EXPECT_EQ(testing_matrix.getCols(), 13);
  EXPECT_EQ(testing_matrix.getRows(), 10);
  for (int i = 0; i<testing_matrix.getRows(); i++) {
    for (int j = 0; j<testing_matrix.getCols(); j++) {
      if(j==12) EXPECT_FLOAT_EQ(testing_matrix.getValue(i,j), col[i]);
      else EXPECT_FLOAT_EQ(testing_matrix.getValue(i,j), 100);
    }
  }
}

TEST_F(MatrixTesting,testAddRowEndQuick){
  std::vector<double> row = {1,2,3,4,5,6,7,8,9,10,11,12};
  Matrix<double> testing_matrix = *m2_;
  testing_matrix.addRowAtTheEnd(row);
  EXPECT_EQ(testing_matrix.getCols(), 12);
  EXPECT_EQ(testing_matrix.getRows(), 11);
  for (int i = 0; i<testing_matrix.getRows(); i++) {
    for (int j = 0; j<testing_matrix.getCols(); j++) {
      if(i==10) EXPECT_FLOAT_EQ(testing_matrix.getValue(i,j), row[j]);
      else EXPECT_FLOAT_EQ(testing_matrix.getValue(i,j), 100);
    }
  }
}