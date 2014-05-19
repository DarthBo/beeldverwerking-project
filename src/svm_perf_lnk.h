#ifndef _H_SVM_LNK
#define _H_SVM_LNK

#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif
#include "svm_perf/svm_light/svm_common.h"
#ifdef __cplusplus
}
#endif
#include "svm_perf/svm_struct_api.h"
#include "svm_perf/svm_struct/svm_struct_common.h"

int svm_struct_classify_main ()
{
  const char* modelfile = "../model/gras.model";
  long correct=0,incorrect=0,no_accuracy=0;
  long i;
  double t1,runtime=0;
  double avgloss=0,l;
  FILE *predfl;
  STRUCTMODEL model;
  STRUCT_LEARN_PARM sparm;
  STRUCT_TEST_STATS teststats;
  SAMPLE testsample;
  LABEL y;


  //svm_struct_classify_api_init(argc,argv);

  sparm.custom_argc=0;
  parse_struct_parameters_classify(struct_parm);


  model=read_struct_model(modelfile,&sparm);

  if(model.svm_model->kernel_parm.kernel_type == LINEAR) { /* linear kernel */
    /* compute weight vector */
    add_weight_vector_to_linear_model(model.svm_model);
    model.w=model.svm_model->lin_weights;
  }

  testsample=read_struct_examples(testfile,&sparm);


  if ((predfl = fopen (predictionsfile, "w")) == NULL)
  { perror (predictionsfile); exit (1); }

  for(i=0;i<testsample.n;i++) {
    t1=get_runtime();
    y=classify_struct_example(testsample.examples[i].x,&model,&sparm);
    runtime+=(get_runtime()-t1);

    write_label(predfl,y);
    l=loss(testsample.examples[i].y,y,&sparm);
    avgloss+=l;
    if(l == 0)
      correct++;
    else
      incorrect++;
    eval_prediction(i,testsample.examples[i],y,&model,&sparm,&teststats);

    if(empty_label(testsample.examples[i].y))
      { no_accuracy=1; } /* test data is not labeled */
    if(struct_verbosity>=2) {
      if((i+1) % 100 == 0) {
    printf("%ld..",i+1); fflush(stdout);
      }
    }
    free_label(y);
  }
  avgloss/=testsample.n;
  fclose(predfl);

  if(struct_verbosity>=1) {
    printf("done\n");
    printf("Runtime (without IO) in cpu-seconds: %.2f\n",
       (float)(runtime/100.0));
  }
  if((!no_accuracy) && (struct_verbosity>=1)) {
    printf("Average loss on test set: %.4f\n",(float)avgloss);
    printf("Zero/one-error on test set: %.2f%% (%ld correct, %ld incorrect, %d total)\n",(float)100.0*incorrect/testsample.n,correct,incorrect,testsample.n);
  }
  print_struct_testing_stats(testsample,&model,&sparm,&teststats);
  free_struct_sample(testsample);
  free_struct_model(model);

  svm_struct_classify_api_exit();

  return(0);
}


#endif
