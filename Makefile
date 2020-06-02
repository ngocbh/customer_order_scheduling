CXX = g++

CFLAGS += -std=c++11 -fPIC
LDFLAGS = -lm -Wno-unused-result -Wno-sign-compare -Wno-unused-variable -Wno-parentheses -Wno-format -Wno-unused-command-line-argument 

ORTOOLS_ROOT = libs/or-tools
ORTOOLS_LIBS = -Wl,-rpath,$(ORTOOLS_ROOT)/lib -L$(ORTOOLS_ROOT)/lib -lprotobuf -lglog -lgflags -lCbcSolver -lCbc -lOsiCbc -lCgl -lClpSolver -lClp -lOsiClp -lOsi -lCoinUtils -lortools -Wl,-rpath,@loader_path -Wl,-rpath,@loader_path/../lib -lz -framework CoreFoundation
ORTOOLS_CFLAGS = $(CFLAGS) -O4 -I$(ORTOOLS_ROOT)/include -I$(ORTOOLS_ROOT) -DARCH_K8 -Wno-deprecated -DUSE_CBC -DUSE_CLP -DUSE_BOP -DUSE_GLO

GECODE_LIBS = -F/Library/Frameworks -framework gecode

CP = ./bin/cosp_cp_naive ./bin/cosp_cp_greedy ./bin/cosp_cp_branching
MIP = ./bin/cosp_mip_weighted_sum ./bin/cosp_mip_greedy ./bin/cosp_mip_two_phase
LS = ./bin/cosp_ls
GREEDY = ./bin/cosp_greedy

.PHONY: clean all

all: bin cp mip ls greedy

cp: $(CP)

mip: $(MIP)

ls: $(LS)

greedy: $(GREEDY)

$(CP) : ./src/cp/cosp_cp_naive.cpp ./src/cp/cosp_cp_greedy.cpp ./src/cp/cosp_cp_branching.cpp ./src/cp/*.h ./src/model/*.h

$(MIP) : ./src/mip/cosp_mip_weighted_sum.cpp ./src/mip/cosp_mip_two_phase.cpp ./src/mip/cosp_mip_greedy.cpp ./src/model/*.h

$(LS) : ./src/ls/cosp_ls.cpp ./src/ls/*.h ./src/model/*.h

$(GREEDY): ./src/greedy/cosp_greedy.cpp ./src/model/*.h

bin/cosp_mip% : bin/cosp_mip%.o 
	@echo "Compling $@"
	$(CXX) $(ORTOOLS_CFLAGS) $(LDFLAGS) $(ORTOOLS_LIBS) $< -o $@ 

bin/cosp_cp% : bin/cosp_cp%.o
	@echo "Compling $@"
	$(CXX) $(CFLAGS) $(LDFLAGS) $(GECODE_LIBS) $< -o $@ 

bin/cosp_ls : bin/cosp_ls.o
	@echo "Compling $@"
	$(CXX) $(CFLAGS) $(LDFLAGS) $< -o $@ 

bin/cosp_greedy : bin/cosp_greedy.o
	echo "Compling $@"
	$(CXX) $(CFLAGS) $(LDFLAGS) $< -o $@ 

bin/%.o : src/mip/%.cpp ./src/model/*.h
	@echo "Compling $@"
	$(CXX) $(ORTOOLS_CFLAGS) $(ORTOOLS_LIBS) $(LDFLAGS) -c $< -o $@

bin/%.o : src/cp/%.cpp ./src/model/*.h ./src/cp/*.h
	@echo "Compling cp $@"
	$(CXX) $(CFLAGS) $(GECODE_LIBS) $(LDFLAGS) -c $< -o $@

bin/%.o : src/ls/%.cpp ./src/model/*.h
	@echo "Compling ls $@"
	$(CXX) $(CFLAGS) $(LDFLAGS) -c $< -o $@

bin/%.o : src/greedy/%.cpp ./src/model/*.h
	@echo "Compling ls $@"
	$(CXX) $(CFLAGS) $(LDFLAGS) -c $< -o $@

bin:
	mkdir -p bin

clean :
	rm -rf bin