Bool_t checkVectorDiffs (vector<Float_t> vec, Float_t test, Float_t window) {
	
	Bool_t result = false;
	Int_t n = vec.size();
	if(n=0) {
		cout << "Size 0 in checkVectorDiffs! Returning false! \n";
		return false;
	}
	
	for(Int_t i=0; i<n; i++) {
		
		if (abs(vec[i]-test)<=window) result = true;
		
	}
	
	return result;
	
}

Int_t findMax (vector<Float_t> vec) {
	
		Int_t imax = 0;
		Int_t n = vec.size();
		Float_t max = 0.0;
		
		for (Int_t i=0; i<n; i++) {
			
			if (vec[i]>max) {
				
				imax=i;
				max=vec[i];
				
			}
			
		}
		
		return imax;
	
}

class JansClusterFinder {
	
	protected:
	
		Float_t timeWindow = 5.0;
	
		vector<Int_t> hitModules;
		vector<Int_t> hitStrips;
		vector<Float_t> hitTots;
		vector<Float_t> hitTimes;
		
		vector<Int_t> clusterModules;
		vector<Int_t> clusterStrips;
		vector<Float_t> clusterTots;
		vector<Float_t> clusterTimes;
		vector<Float_t> clusterSizes;
		
	public:

		JansClusterFinder (Float_t tw = 5.0) {
			
			JansClusterFinder::timeWindow = tw;
			
		}
		
		~JansClusterFinder() {}
		
		void addHit(Int_t module, Int_t strip, Float_t ToT, Float_t time) {
			
			hitModules.push_back(module);
			hitStrips.push_back(strip);
			hitTots.push_back(ToT);
			hitTimes.push_back(time);
			
		}
		
		void findClusters() {
			
			Int_t nHits = hitModules.size();
			
			if (nHits<1) {
				cout << "not enough hits to look for clusters !!!" << endl;
				return;
			}
			
			for (Int_t i = 0; i<nHits; i++) {
			
				// get empty vectors for a cluster candidate
				vector<Int_t> clusterCandidateModules;
				vector<Int_t> clusterCandidateStrips;
				vector<Float_t> clusterCandidateTots;
				vector<Float_t> clusterCandidateTimes;
				Int_t clusterCandidateSize = 0;
				
				// choose a time ref and iterate over all hits, add matching ones to the cluster candidate vector
				Float_t refTime = 0.0;
				if (hitTimes[i]!=-9999.0) refTime = hitTimes[i];
				else continue;
				
				while(true) { // actually - keep iterating over all the hits, until you find a match
					
					Int_t addedClusters = 0;
					
					for (Int_t j = 0; j<nHits; j++) {
						
						if ((abs(refTime-hitTimes[j])<timeWindow || 
						checkVectorDiffs(clusterCandidateTimes, hitTimes[j], timeWindow)) && 
						hitTimes[j]!=-9999.0) { // include a hit that simply fits with our ref time, or that matches with any of the hits in cluster candidate, reject hits already included in a cluster candidate
							
							clusterCandidateModules.push_back(hitModules[j]);
							clusterCandidateStrips.push_back(hitStrips[j]);
							clusterCandidateTots.push_back(hitTots[j]);
							clusterCandidateTimes.push_back(hitTimes[j]);
							clusterCandidateSize++;
							addedClusters++;
							
							// if a hit was added to a cluster - delete it from available hits
							hitModules[j]=-9999.0;
							hitStrips[j]=-9999.0;
							hitTots[j]=-9999.0;
							hitTimes[j]=-9999.0;
							
						}
						
					}
					
					if (addedClusters == 0) break;
					
				}
				
				Int_t imax = findMax(clusterCandidateTots); // find the hit with max tot within this cluster
				
				clusterModules.push_back(clusterCandidateModules[imax]);
				clusterStrips.push_back(clusterCandidateStrips[imax]);
				clusterTots.push_back(clusterCandidateTots[imax]);
				clusterTimes.push_back(clusterCandidateTimes[imax]);
				clusterSizes.push_back(clusterCandidateSize);
				
				
				//cout << "Found cluster with size " << clusterCandidateSize << ", tot=" << clusterCandidateTots[imax] << ", time=" << clusterCandidateTimes[imax] << endl;
				
			}
			
		}
		
		Int_t getNumberOfClusters () {return clusterModules.size();}
		Int_t getClusterSize (Int_t i) {if (clusterModules.size()>0) return clusterSizes[i]; else return 0;}
		Float_t getClusterModule (Int_t i) {if (clusterModules.size()>0) return clusterModules[i]; else return -999.0;}
		Float_t getClusterStrip (Int_t i) {if (clusterModules.size()>0) return clusterStrips[i]; else return -999.0;}
		Float_t getClusterTime (Int_t i) {if (clusterModules.size()>0) return clusterTimes[i]; else return -999.0;}
		Float_t getClusterTot (Int_t i) {if (clusterModules.size()>0) return clusterTots[i]; else return -999.0;}
		
};
