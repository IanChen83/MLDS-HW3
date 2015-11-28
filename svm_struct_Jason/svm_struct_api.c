/***********************************************************************/
/*                                                                     */
/*   svm_struct_api.c                                                  */
/*                                                                     */
/*   Definition of API for attaching implementing SVM learning of      */
/*   structures (e.g. parsing, multi-label classification, HMM)        */ 
/*                                                                     */
/*   Author: Thorsten Joachims                                         */
/*   Date: 03.07.04                                                    */
/*                                                                     */
/*   Copyright (c) 2004  Thorsten Joachims - All rights reserved       */
/*                                                                     */
/*   This software is available for non-commercial use only. It must   */
/*   not be modified and distributed without prior permission of the   */
/*   author. The author is not responsible for implications from the   */
/*   use of this software.                                             */
/*                                                                     */
/***********************************************************************/


/***********************************************************************/
/*                                                                     */
/*   svm_struct_api.c                                                  */
/*                                                                     */
/*   Definition of API for attaching implementing SVM learning of      */
/*   structures (e.g. parsing, multi-label classification, HMM)        */ 
/*                                                                     */
/*   Editor: Chien-Shen Wu                                             */
/*   Date: 11.30.2015                                                  */
/*                                                                     */
/***********************************************************************/

#include <stdio.h>
#include <string.h>
#include "svm_struct/svm_struct_common.h"
#include "svm_struct_api.h"


# define OBSERVE_ELEMENT_DIM 48 
# define STATE_TYPES 48
# define MAX_ELEMENT_IN_OBSERVE 777  // should be modified

// viterbi algorithm
LABEL viterbi(PATTERN x, LABEL y, STRUCTMODEL *sm, int loss_bool)
{
  
  // record the path (end in last state) points to this state will make max value
  int viterbi_table[x.frame_num][STATE_TYPES]; // t from 1 to x.frame_num
  float max_val_now[STATE_TYPES];
  float max_val_last[STATE_TYPES];

  //initial table
  int w;
  int j;
  for (j = 0; j < x.frame_num; j++){
    for (w = 0; w < STATE_TYPES; ++w)
      viterbi_table[j][w] = 0;
  }
  for (w = 0; w < STATE_TYPES; ++w){
    max_val_now[w] = 0;
    max_val_last[w] = 0;
  }

  double* Wobserve = & ( sm->w[1] );
  double* Wtrans = & ( sm->w[OBSERVE_ELEMENT_DIM*STATE_TYPES  + 1] ); 
  // + OBSERVE_ELEMENT_DIM : add dummy hint

  //printf(" w[OBSERVE_ELEMENT_DIM*STATE_TYPES + 1] : %lf \n", sm->w[OBSERVE_ELEMENT_DIM*STATE_TYPES + 2]);
  int t;
  for (t = 0; t < x.frame_num; ++t){
    int s;
    // observation element in time t
    if (t == 0){
      for (s = 0; s < STATE_TYPES; ++s){
        // for each state type s
        // Wobserve*psi
        int i;
        for (i = 0; i < OBSERVE_ELEMENT_DIM; ++i)
          max_val_now[s] += Wobserve[s*OBSERVE_ELEMENT_DIM + i]*x.observe_object[t][i];

        if (loss_bool)
        {
          if (s != y.state_object[t])
          //  max_val_now[s] += 1.0/(double)(y.frame_num);
            max_val_now[s] ++;
        }
      }
    }
    else{
      for (s = 0; s < STATE_TYPES; ++s){
        // for each state type s
        double temp_max_value = -1000000;
        int max_path_index = 0;

        int p;
        // for every path p cal max_value(p) last time + Wtrans*psi@[a -> b] 
        for (p = 0; p < STATE_TYPES; ++p){
          double path_value = 0; 
    
          path_value = max_val_last[p] + Wtrans[p*STATE_TYPES + s]*1.0 ; // ex : a->b

          if (path_value > temp_max_value){
            temp_max_value = path_value;
            max_path_index = p;
          }
        }
        // build viterbi table
        viterbi_table[t][s] = max_path_index; 
        max_val_now[s] = temp_max_value;

        // add Wobserve*psi @ time t
        int i;
        for (i = 0; i < OBSERVE_ELEMENT_DIM; ++i)
          max_val_now[s] += Wobserve[s*OBSERVE_ELEMENT_DIM + i]*x.observe_object[t][i];

        if (loss_bool){
          if (s != y.state_object[t])
           // max_val_now[s]  += 1.0/(double)(y.frame_num);
            max_val_now[s]++;
        }
      }
    }
//printf("time %d:\n",t);
    for (w = 0; w < STATE_TYPES; ++w)
    {
      //printf("  %d %d \n",w, max_val_now[w]);
      max_val_last[w] = max_val_now[w];
    }

  }

  // find max path
  int max_end_index = 0;
  float max = -1000000;

  int s;
  for (s = 0; s < STATE_TYPES; ++s){
    if (max_val_now[s] > max)
    {
      max = max_val_now[s];
      max_end_index = s;
    }
  }
 //printf("max : %f \n", max);
  // back trace max path and build y_max
  LABEL y_max;
  y_max.state_object = (int *)malloc(sizeof(int)*MAX_ELEMENT_IN_OBSERVE);
  y_max.speaker = (char *)malloc(sizeof(char)*20);
  y_max.frame_num = x.frame_num;

  y_max.state_object[y_max.frame_num - 1] = max_end_index;
  int predict_state = max_end_index;

  for (t = x.frame_num - 1; t > 0; --t){
    predict_state = viterbi_table[t][predict_state];
    y_max.state_object[t-1] = predict_state;
  }

  return (y_max);
}


void        svm_struct_learn_api_init(int argc, char* argv[])
{
  /* Called in learning part before anything else is done to allow
     any initializations that might be necessary. */
}

void        svm_struct_learn_api_exit()
{
  /* Called in learning part at the very end to allow any clean-up
     that might be necessary. */
}

void        svm_struct_classify_api_init(int argc, char* argv[])
{
  /* Called in prediction part before anything else is done to allow
     any initializations that might be necessary. */
}

void        svm_struct_classify_api_exit()
{
  /* Called in prediction part at the very end to allow any clean-up
     that might be necessary. */
}

SAMPLE      read_struct_examples(char *file, STRUCT_LEARN_PARM *sparm)
{
  /* Reads struct examples and returns them in sample. The number of
     examples must be written into sample.n */
  SAMPLE   sample;  /* sample */
  EXAMPLE  *examples;
  long     n;       /* number of examples */
  n = 0;
  
  // TODO
  int i;
  FILE *read_file;
  if((read_file = fopen(file, "r")) == NULL){
        printf("read_struct_example %s error!!\n", file);
        exit(0);
  }
  
  char line_buf[1024];
  char *speaker_idx, *sentence_idx, *frame_idx;
  char last_speaker[20];
  // count n
  while(fgets(line_buf, 1023, read_file) != NULL) {
    speaker_idx = strtok(line_buf, "_");
    sentence_idx = strtok(NULL, "_");
    frame_idx = strtok(NULL, " ");
    if (strcmp(frame_idx, "1") == 0) {
      n++;
    }
  }

  // n = 500; // debug use

  fclose(read_file);
  
  //n=0; /* replace by appropriate number of examples */
  examples=(EXAMPLE *)my_malloc(sizeof(EXAMPLE)*n);

  for(i = 0; i < n ;i ++){

    examples[i].x.speaker = (char*)malloc(sizeof(char)*20);
    examples[i].y.speaker = (char*)malloc(sizeof(char)*20);

    examples[i].x.observe_object = (float**)malloc(sizeof(float*)*MAX_ELEMENT_IN_OBSERVE);

    int t;
    for (t = 0; t < MAX_ELEMENT_IN_OBSERVE; ++t)
    {
      examples[i].x.observe_object[t] = (float*)malloc(sizeof(float)*OBSERVE_ELEMENT_DIM);
    }
    
    examples[i].y.state_object = (int*)malloc(sizeof(int)*MAX_ELEMENT_IN_OBSERVE);
  }

  /* fill in your code here */
  if((read_file = fopen(file, "r")) == NULL){
        printf("read_struct_example %s error!!\n", file);
        exit(0);
  }
  int sentence_id = -1;
  int frame_id, last_frame_id;
  while(fgets(line_buf, 1023, read_file) != NULL) {
    speaker_idx = strtok(line_buf, "_");
    sentence_idx = strtok(NULL, "_");
    frame_idx = strtok(NULL, " ");
    frame_id = atoi(frame_idx);
    
    if (frame_id == 1) {
      if (sentence_id >= 0) {
        examples[sentence_id].x.frame_num = last_frame_id;
        examples[sentence_id].y.frame_num = last_frame_id;
        strcpy(examples[sentence_id].x.speaker, last_speaker);
      }
      sentence_id++;
    }
 
    // debug use
/*  if (sentence_id == 500)
      break; */
 
    for (i = 0; i < OBSERVE_ELEMENT_DIM ; i++) {
      examples[sentence_id].x.observe_object[frame_id-1][i] = atof(strtok(NULL, " ")); // string to double 
    }
    // examples[sentence_id].x.observe_object[frame_id-1][OBSERVE_ELEMENT_DIM - 1] = 1.0; // hint

    examples[sentence_id].y.state_object[frame_id-1] = atoi(strtok(NULL, " "));
      
    last_frame_id = frame_id;
    strcpy(last_speaker, speaker_idx);
    strcat(last_speaker, "_");
    strcat(last_speaker, sentence_idx);
  }
  // remember add comment when n is specific

  // for last sentence 
  examples[sentence_id].x.frame_num = last_frame_id;
  examples[sentence_id].y.frame_num = last_frame_id;
  strcpy(examples[sentence_id].x.speaker, last_speaker);

  fclose(read_file);
  
  sample.n=n;
  sample.examples=examples;
  return(sample);
}

void        init_struct_model(SAMPLE sample, STRUCTMODEL *sm, 
            STRUCT_LEARN_PARM *sparm, LEARN_PARM *lparm, 
            KERNEL_PARM *kparm)
{
  /* Initialize structmodel sm. The weight vector w does not need to be
     initialized, but you need to provide the maximum size of the
     feature space in sizePsi. This is the maximum number of different
     weights that can be learned. Later, the weight vector w will
     contain the learned weights for the model. */

  sm->sizePsi= OBSERVE_ELEMENT_DIM*STATE_TYPES + STATE_TYPES*STATE_TYPES; 
  
  // OBSERVE_ELEMENT_DIM < dummy psi feature hint >
}

CONSTSET    init_struct_constraints(SAMPLE sample, STRUCTMODEL *sm, 
            STRUCT_LEARN_PARM *sparm)
{
  /* Initializes the optimization problem. Typically, you do not need
     to change this function, since you want to start with an empty
     set of constraints. However, if for example you have constraints
     that certain weights need to be positive, you might put that in
     here. The constraints are represented as lhs[i]*w >= rhs[i]. lhs
     is an array of feature vectors, rhs is an array of doubles. m is
     the number of constraints. The function returns the initial
     set of constraints. */
  CONSTSET c;
  long     sizePsi=sm->sizePsi;
  long     i;
  WORD     words[2];

  if(1) { /* normal case: start with empty set of constraints */
    c.lhs=NULL;
    c.rhs=NULL;
    c.m=0;
  }
  else { /* add constraints so that all learned weights are
            positive. WARNING: Currently, they are positive only up to
            precision epsilon set by -e. */
    c.lhs=my_malloc(sizeof(DOC *)*sizePsi);
    c.rhs=my_malloc(sizeof(double)*sizePsi);
    for(i=0; i<sizePsi; i++) {
      words[0].wnum=i+1;
      words[0].weight=1.0;
      words[1].wnum=0;
      /* the following slackid is a hack. we will run into problems,
         if we have move than 1000000 slack sets (ie examples) */
      c.lhs[i]=create_example(i,0,1000000+i,1,create_svector(words,"",1.0));
      c.rhs[i]=0.0;
    }
  }
  return(c);
}

LABEL       classify_struct_example(PATTERN x, STRUCTMODEL *sm, 
            STRUCT_LEARN_PARM *sparm)
{
  /* Finds the label yhat for pattern x that scores the highest
     according to the linear evaluation function in sm, especially the
     weights sm.w. The returned label is taken as the prediction of sm
     for the pattern x. The weights correspond to the features defined
     by psi() and range from index 1 to index sm->sizePsi. If the
     function cannot find a label, it shall return an empty label as
     recognized by the function empty_label(y). */

  /* insert your code for computing the predicted label y here */
  // TODO
  LABEL y_max;
  LABEL y; // for Viterbi argument, do nothing
  
  strcpy(y_max.speaker, x.speaker);
  y_max = viterbi (x , y, sm, 0);

/*
  int i;
  for (i = 0; i < y_max.frame_num; ++i)
  {
    printf("%s: %d \n", y_max.speaker, y_max.state_object[i]);
  }*/

  return(y_max);
}

LABEL       find_most_violated_constraint_slackrescaling(PATTERN x, LABEL y, 
                 STRUCTMODEL *sm, 
                 STRUCT_LEARN_PARM *sparm)
{
  /* Finds the label ybar for pattern x that that is responsible for
     the most violated constraint for the slack rescaling
     formulation. For linear slack variables, this is that label ybar
     that maximizes

            argmax_{ybar} loss(y,ybar)*(1-psi(x,y)+psi(x,ybar)) 

     Note that ybar may be equal to y (i.e. the max is 0), which is
     different from the algorithms described in
     [Tschantaridis/05]. Note that this argmax has to take into
     account the scoring function in sm, especially the weights sm.w,
     as well as the loss function, and whether linear or quadratic
     slacks are used. The weights in sm.w correspond to the features
     defined by psi() and range from index 1 to index
     sm->sizePsi. Most simple is the case of the zero/one loss
     function. For the zero/one loss, this function should return the
     highest scoring label ybar (which may be equal to the correct
     label y), or the second highest scoring label ybar, if
     Psi(x,ybar)>Psi(x,y)-1. If the function cannot find a label, it
     shall return an empty label as recognized by the function
     empty_label(y). */
  LABEL ybar;

  /* insert your code for computing the label ybar here */
  // printf("find_most_violated_constraint_slackrescaling is called\n");
  return(ybar);
}

LABEL       find_most_violated_constraint_marginrescaling(PATTERN x, LABEL y, 
                 STRUCTMODEL *sm, 
                 STRUCT_LEARN_PARM *sparm)
{
  /* Finds the label ybar for pattern x that that is responsible for
     the most violated constraint for the margin rescaling
     formulation. For linear slack variables, this is that label ybar
     that maximizes

            argmax_{ybar} loss(y,ybar)+psi(x,ybar)

     Note that ybar may be equal to y (i.e. the max is 0), which is
     different from the algorithms described in
     [Tschantaridis/05]. Note that this argmax has to take into
     account the scoring function in sm, especially the weights sm.w,
     as well as the loss function, and whether linear or quadratic
     slacks are used. The weights in sm.w correspond to the features
     defined by psi() and range from index 1 to index
     sm->sizePsi. Most simple is the case of the zero/one loss
     function. For the zero/one loss, this function should return the
     highest scoring label ybar (which may be equal to the correct
     label y), or the second highest scoring label ybar, if
     Psi(x,ybar)>Psi(x,y)-1. If the function cannot find a label, it
     shall return an empty label as recognized by the function
     empty_label(y). */
  

  LABEL ybar;
 
  /* insert your code for computing the label ybar here */

  // TODO
  //printf("find_most_violated_constraint_marginrescaling is called\n");
  ybar = viterbi (x , y, sm, 1);
  return(ybar);
}

int         empty_label(LABEL y)
{
  /* Returns true, if y is an empty label. An empty label might be
     returned by find_most_violated_constraint_???(x, y, sm) if there
     is no incorrect label that can be found for x, or if it is unable
     to label x at all */
  return(0);
}

SVECTOR     *psi(PATTERN x, LABEL y, STRUCTMODEL *sm,
     STRUCT_LEARN_PARM *sparm)
{
  /* Returns a feature vector describing the match between pattern x
     and label y. The feature vector is returned as a list of
     SVECTOR's. Each SVECTOR is in a sparse representation of pairs
     <featurenumber:featurevalue>, where the last pair has
     featurenumber 0 as a terminator. Featurenumbers start with 1 and
     end with sizePsi. Featuresnumbers that are not specified default
     to value 0. As mentioned before, psi() actually returns a list of
     SVECTOR's. Each SVECTOR has a field 'factor' and 'next'. 'next'
     specifies the next element in the list, terminated by a NULL
     pointer. The list can be though of as a linear combination of
     vectors, where each vector is weighted by its 'factor'. This
     linear combination of feature vectors is multiplied with the
     learned (kernelized) weight vector to score label y for pattern
     x. Without kernels, there will be one weight in sm.w for each
     feature. Note that psi has to match
     find_most_violated_constraint_???(x, y, sm) and vice versa. In
     particular, find_most_violated_constraint_???(x, y, sm) finds
     that ybar!=y that maximizes psi(x,ybar,sm)*sm.w (where * is the
     inner vector product) and the appropriate function of the
     loss + margin/slack rescaling method. See that paper for details. */
  SVECTOR *fvec = NULL;

  WORD words[sm->sizePsi+1];
  /* insert code for computing the feature vector for x and y here */
  int i, j;
  for (i = 0; i < sm->sizePsi; i++) {
    words[i].wnum = i+1;
    words[i].weight = 0.0;
  }
  words[sm->sizePsi].wnum = 0;  // for terminator
  
  // build psi_observation  put into psi_feature[label]
  for (i = 0; i < x.frame_num; i++) {

     for (j = 0; j < OBSERVE_ELEMENT_DIM; j++) {
        words[y.state_object[i]*OBSERVE_ELEMENT_DIM + j].weight += x.observe_object[i][j];
     }
  }

  // build psi_transition  put 1 into psi_feature[labeli->labelj] if i->j
  for (i = 1; i < x.frame_num; i++) {
    words[OBSERVE_ELEMENT_DIM*STATE_TYPES + y.state_object[i-1]*STATE_TYPES + y.state_object[i]].weight += 1.0;
  }

  fvec = create_svector(words, 0, 1.0);
  //printf("psi function is finished.\n");
  return(fvec);
}

double      loss(LABEL y, LABEL ybar, STRUCT_LEARN_PARM *sparm)
{
  //printf("loss function is called.\n");
  /* loss for correct label y and predicted label ybar. The loss for
     y==ybar has to be zero. sparm->loss_function is set with the -l option. */
  int i;
  int error_count = 0;

  if(sparm->loss_function == 0) { /* type 0 loss: 0/1 loss */
                                  /* return 0, if y==ybar. return 1 else */
    for (i = 0; i < y.frame_num; i++) {
      if (y.state_object[i] != ybar.state_object[i]){
         error_count++;
      }
        
    }
  }
  else {
    /* Put your code for different loss functions here. But then
       find_most_violated_constraint_???(x, y, sm) has to return the
       highest scoring label with the largest loss. */
  }
  //printf("loss : %d \n", error_count);
  // (double)(error_count)/(double)(y.frame_num)
  return (double)(error_count);
}

int         finalize_iteration(double ceps, int cached_constraint,
             SAMPLE sample, STRUCTMODEL *sm,
             CONSTSET cset, double *alpha, 
             STRUCT_LEARN_PARM *sparm)
{
  /* This function is called just before the end of each cutting plane iteration. ceps is the amount by which the most violated constraint found in the current iteration was violated. cached_constraint is true if the added constraint was constructed from the cache. If the return value is FALSE, then the algorithm is allowed to terminate. If it is TRUE, the algorithm will keep iterating even if the desired precision sparm->epsilon is already reached. */
  return(0);
}

void        print_struct_learning_stats(SAMPLE sample, STRUCTMODEL *sm,
          CONSTSET cset, double *alpha, 
          STRUCT_LEARN_PARM *sparm)
{
  /* This function is called after training and allows final touches to
     the model sm. But primarly it allows computing and printing any
     kind of statistic (e.g. training error) you might want. */
}

void        print_struct_testing_stats(SAMPLE sample, STRUCTMODEL *sm,
               STRUCT_LEARN_PARM *sparm, 
               STRUCT_TEST_STATS *teststats)
{
  /* This function is called after making all test predictions in
     svm_struct_classify and allows computing and printing any kind of
     evaluation (e.g. precision/recall) you might want. You can use
     the function eval_prediction to accumulate the necessary
     statistics for each prediction. */
}

void        eval_prediction(long exnum, EXAMPLE ex, LABEL ypred, 
          STRUCTMODEL *sm, STRUCT_LEARN_PARM *sparm, 
          STRUCT_TEST_STATS *teststats)
{
  /* This function allows you to accumlate statistic for how well the
     predicition matches the labeled example. It is called from
     svm_struct_classify. See also the function
     print_struct_testing_stats. */
  if(exnum == 0) { /* this is the first time the function is
          called. So initialize the teststats */
  }
}

void        write_struct_model(char *file, STRUCTMODEL *sm, 
             STRUCT_LEARN_PARM *sparm)
{
  /* Writes structural model sm to file file. */
  FILE *model;
  model = fopen(file, "w");
  int i;
  
  // sizePsi
  fprintf(model, "%ld\n", sm->sizePsi+1);  // write w[0]
  // weight 
  for (i = 0; i < sm->sizePsi+1; i++) {
    fprintf(model, "%lf\n", sm->w[i]);
  }
  fprintf(model, "\n");
  
  fclose(model);
}

STRUCTMODEL read_struct_model(char *file, STRUCT_LEARN_PARM *sparm)
{
  /* Reads structural model sm from file file. This function is used
     only in the prediction module, not in the learning module. */
  

  STRUCTMODEL sm;
  sm.svm_model = (MODEL *)malloc(sizeof(MODEL));
  
  FILE *model;
  if((model = fopen(file, "r")) == NULL){
    printf("read_struct_model error!!\n");
        exit(0);
  }
  
  int i;
  long sizeWeight;
   
  fscanf(model, "%ld", &sizeWeight);
  sm.sizePsi = sizeWeight - 1;
  sm.w = (double*)malloc(sizeof(double)*sizeWeight);

  for (i = 0; i < sizeWeight; i++) {
    fscanf(model, "%lf", &(sm.w[i]));
  }
  return sm;
}

void        write_label(FILE *fp, LABEL y)
{

  /* Writes label y to file handle fp. */
  fprintf(fp, "%s,", y.speaker);
  int i = 0;
  while(y.state_object[i] == 36  || y.state_object[i] == 9 || y.state_object[i] == 16 ||y.state_object[i] == 42) {  // skip the leading sil's
    i++;
  }
  
  for (i; i < y.frame_num - 1; i++) {
    if (y.state_object[i] == y.state_object[i+1]) {
      continue;
    }
    // 39 phones but map to character according to 48_idx_chr
    switch (y.state_object[i]) {
      case 0 :
        fprintf(fp, "a");
        break;
      case 1 :
        fprintf(fp, "b");
        break;
      case 2 :
        fprintf(fp, "c"); 
        break;
      case 3 :
        fprintf(fp, "a"); // d
        break;
      case 4 :
        fprintf(fp, "e");
        break;
      case 5 :
        fprintf(fp, "c"); // f
        break;
      case 6 :
        fprintf(fp, "g");
        break;
      case 7 :
        fprintf(fp, "h");
        break;
      case 8 :
        fprintf(fp, "i");
        break;
      case 9 :
        fprintf(fp, "L"); // j
        break;
      case 10 :
        fprintf(fp, "k");
        break;
      case 11 :
        fprintf(fp, "l");
        break;
      case 12 :
        fprintf(fp, "m");
        break;
      case 13 :
        fprintf(fp, "n");
        break;
      case 14 :
        fprintf(fp, "B"); // o
        break;
      case 15 :
        fprintf(fp, "D"); // p
        break;
      case 16 :
        fprintf(fp, "L"); // q
        break;
      case 17 :
        fprintf(fp, "r");
        break;
      case 18 :
        fprintf(fp, "s");
        break;
      case 19 :
        fprintf(fp, "t");
        break;
      case 20 :
        fprintf(fp, "u");
        break;
      case 21 :
        fprintf(fp, "v");
        break;
      case 22 :
        fprintf(fp, "w");
        break;
      case 23 :
        fprintf(fp, "w"); // x
        break;
      case 24 :
        fprintf(fp, "y");
        break;
      case 25 :
        fprintf(fp, "z");
        break;
      case 26 :
        fprintf(fp, "A");
        break;
      case 27 :
        fprintf(fp, "B");
        break;
      case 28 :
        fprintf(fp, "C");
        break;
      case 29 :
        fprintf(fp, "E");
        break;
      case 30 :
        fprintf(fp, "D");
        break;
      case 31 :
        fprintf(fp, "F");
        break;
      case 32 :
        fprintf(fp, "G");
        break;
      case 33 :
        fprintf(fp, "H");
        break;
      case 34 :
        fprintf(fp, "I");
        break;
      case 35 :
        fprintf(fp, "K");
        break;
      case 36 :
        fprintf(fp, "L");
        break;
      case 37 :
        fprintf(fp, "J");
        break;
      case 38 :
        fprintf(fp, "N");
        break;
      case 39 :
        fprintf(fp, "M");
        break;
      case 40 :
        fprintf(fp, "O");
        break;
      case 41 :
        fprintf(fp, "P");
        break;
      case 42 :
        fprintf(fp, "L");
        break;
      case 43 :
        fprintf(fp, "Q"); // R
        break;
      case 44 :
        fprintf(fp, "S");
        break;
      case 45 :
        fprintf(fp, "T");
        break;
      case 46 :
        fprintf(fp, "K");
        break;
      case 47 :
        fprintf(fp, "U"); // V
        break;
      default :
        fprintf(fp, "Z");  // error!!
        break;
    }
  }
  
  if (y.state_object[y.frame_num - 2] == y.state_object[y.frame_num - 1]) {
      switch (y.state_object[i]) {
      case 0 :
        fprintf(fp, "a");
        break;
      case 1 :
        fprintf(fp, "b");
        break;
      case 2 :
        fprintf(fp, "c"); 
        break;
      case 3 :
        fprintf(fp, "a"); // d
        break;
      case 4 :
        fprintf(fp, "e");
        break;
      case 5 :
        fprintf(fp, "c"); // f
        break;
      case 6 :
        fprintf(fp, "g");
        break;
      case 7 :
        fprintf(fp, "h");
        break;
      case 8 :
        fprintf(fp, "i");
        break;
      case 9 :
        
        break;
      case 10 :
        fprintf(fp, "k");
        break;
      case 11 :
        fprintf(fp, "l");
        break;
      case 12 :
        fprintf(fp, "m");
        break;
      case 13 :
        fprintf(fp, "n");
        break;
      case 14 :
        fprintf(fp, "B"); // o
        break;
      case 15 :
        fprintf(fp, "D"); // p
        break;
      case 16 :
        
        break;
      case 17 :
        fprintf(fp, "r");
        break;
      case 18 :
        fprintf(fp, "s");
        break;
      case 19 :
        fprintf(fp, "t");
        break;
      case 20 :
        fprintf(fp, "u");
        break;
      case 21 :
        fprintf(fp, "v");
        break;
      case 22 :
        fprintf(fp, "w");
        break;
      case 23 :
        fprintf(fp, "w"); // x
        break;
      case 24 :
        fprintf(fp, "y");
        break;
      case 25 :
        fprintf(fp, "z");
        break;
      case 26 :
        fprintf(fp, "A");
        break;
      case 27 :
        fprintf(fp, "B");
        break;
      case 28 :
        fprintf(fp, "C");
        break;
      case 29 :
        fprintf(fp, "E");
        break;
      case 30 :
        fprintf(fp, "D");
        break;
      case 31 :
        fprintf(fp, "F");
        break;
      case 32 :
        fprintf(fp, "G");
        break;
      case 33 :
        fprintf(fp, "H");
        break;
      case 34 :
        fprintf(fp, "I");
        break;
      case 35 :
        fprintf(fp, "K");
        break;
      case 36 :
       
        break;
      case 37 :
        fprintf(fp, "J");
        break;
      case 38 :
        fprintf(fp, "N");
        break;
      case 39 :
        fprintf(fp, "M");
        break;
      case 40 :
        fprintf(fp, "O");
        break;
      case 41 :
        fprintf(fp, "P");
        break;
      case 42 :
        
        break;
      case 43 :
        fprintf(fp, "Q"); // R
        break;
      case 44 :
        fprintf(fp, "S");
        break;
      case 45 :
        fprintf(fp, "T");
        break;
      case 46 :
        fprintf(fp, "K");
        break;
      case 47 :
        fprintf(fp, "U"); // V
        break;
      default :
        fprintf(fp, "Z");  // error!!
        break;
    }
  }
  fprintf(fp, "\n");
} 

void        free_pattern(PATTERN x) {
  /* Frees the memory of x. */
  // TODO
  //printf("free_pattern is called \n");
  int t;
  for (t = 0; t < MAX_ELEMENT_IN_OBSERVE; ++t)
  {
    free(x.observe_object[t]);
  }

  free(x.observe_object);
  free(x.speaker);
}

void        free_label(LABEL y) {
  /* Frees the memory of y. */
  // TODO
  //printf("free_label is called \n");
  free(y.state_object);
  free(y.speaker);
}

void        free_struct_model(STRUCTMODEL sm) 
{
  /* Frees the memory of model. */
  /* if(sm.w) free(sm.w); */ /* this is free'd in free_model */
  if(sm.svm_model) free_model(sm.svm_model,1);
  /* add free calls for user defined data here */
}

void        free_struct_sample(SAMPLE s)
{
  /* Frees the memory of sample s. */
  // printf("free_struct_sample is called \n");
  int i;
  for(i=0;i<s.n;i++) { 
    free_pattern(s.examples[i].x);
    free_label(s.examples[i].y);
  }
  free(s.examples);
}

void        print_struct_help()
{
  /* Prints a help text that is appended to the common help text of
     svm_struct_learn. */
  printf("         --* string  -> custom parameters that can be adapted for struct\n");
  printf("                        learning. The * can be replaced by any character\n");
  printf("                        and there can be multiple options starting with --.\n");
}

void        parse_struct_parameters(STRUCT_LEARN_PARM *sparm)
{
  /* Parses the command line parameters that start with -- */
  int i;

  for(i=0;(i<sparm->custom_argc) && ((sparm->custom_argv[i])[0] == '-');i++) {
    switch ((sparm->custom_argv[i])[2]) 
      { 
      case 'a': i++; /* strcpy(learn_parm->alphafile,argv[i]); */ break;
      case 'e': i++; /* sparm->epsilon=atof(sparm->custom_argv[i]); */ break;
      case 'k': i++; /* sparm->newconstretrain=atol(sparm->custom_argv[i]); */ break;
      default: printf("\nUnrecognized option %s!\n\n",sparm->custom_argv[i]);
         exit(0);
      }
  }
}

void        print_struct_help_classify()
{
  /* Prints a help text that is appended to the common help text of
     svm_struct_classify. */
  printf("         --* string -> custom parameters that can be adapted for struct\n");
  printf("                       learning. The * can be replaced by any character\n");
  printf("                       and there can be multiple options starting with --.\n");
}

void         parse_struct_parameters_classify(STRUCT_LEARN_PARM *sparm)
{
  /* Parses the command line parameters that start with -- for the
     classification module */
  int i;

  for(i=0;(i<sparm->custom_argc) && ((sparm->custom_argv[i])[0] == '-');i++) {
    switch ((sparm->custom_argv[i])[2]) 
      { 
      /* case 'x': i++; strcpy(xvalue,sparm->custom_argv[i]); break; */
      default: printf("\nUnrecognized option %s!\n\n",sparm->custom_argv[i]);
         exit(0);
      }
  }
}



