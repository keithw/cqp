AM_CPPFLAGS = $(CXX14_FLAGS) $(PANGOCAIRO_CFLAGS) $(GTKMM_CFLAGS)
AM_CXXFLAGS = $(PICKY_CXXFLAGS)

noinst_LIBRARIES = libfader.a

common_source = fader.hh tracker.hh tracker.cc input.hh planner.hh planner.cc machines.hh \
	model_runner.hh model_runner.cc exception.hh

libfader_a_SOURCES = fader.cc
libfader_a_CXXFLAGS = $(TOLERANT_CXXFLAGS)

bin_PROGRAMS = cqp

cqp_SOURCES = cqp.cc $(common_source)
cqp_LDADD = libfader.a $(PANGOCAIRO_LIBS) $(GTKMM_LIBS) -lpthread
