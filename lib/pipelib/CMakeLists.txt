
cmake_minimum_required(VERSION 2.6)

set(pipelib_Header_Files__
  pipelib/AbstractLinkPipeline.h
  pipelib/AbstractPipeline.h
  pipelib/AbstractSimpleBarrier.h
  pipelib/AbstractSimpleBlock.h
  pipelib/AbstractSimpleStartBlock.h
  pipelib/Barrier.h
  pipelib/Block.h
  pipelib/DefaultBarrier.h
  pipelib/DirectPipeline.h
  pipelib/EmptyGlobalData.h
  pipelib/IDataSink.h
  pipelib/IFinishNotifiable.h
  pipelib/ILink.h
  pipelib/IPipeline.h
  pipelib/PipeBlock.h
  pipelib/Pipeline.h
  pipelib/SimpleEndBlock.h
  pipelib/SimpleLink.h
  pipelib/SingleThreadedPipeline.h
  pipelib/StartBlock.h
)
source_group("Header Files\\" FILES ${pipelib_Header_Files__})

set(pipelib_Header_Files
  ${pipelib_Header_Files__}
)


set(pipelib_Files
  ${pipelib_Header_Files}
  build/Dummy.cpp      # Have to have a cpp file to make add_library work
)


add_library(pipelib ${pipelib_Files})