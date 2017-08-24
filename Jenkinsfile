properties([
		parameters([
				choice(choices: 'Debug\nRelease\nRelWithDebInfo\nMinSizeRel', description: 'CMake build type', name: 'BUILD_TYPE')
		])
])

pipeline {
	agent {
		label 'qt && cmake'
	}
	stages {
		stage('Build') {
			steps {
				sh '''
					mkdir build
					cd build
					cmake .. -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
					make -j
				'''
			}
		}
		stage('Deploy') {
			when {
				environment name: 'BUILD_TYPE', value: 'Release'
			}
			steps {
				archiveArtifacts artifacts: 'build/bin/**', onlyIfSuccessful: true
			}
		}
	}
}
