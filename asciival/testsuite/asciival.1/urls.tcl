
# $Id: urls.tcl,v 1.1 1999/05/25 15:46:34 jimg Exp $

# Datasets and their expected output (the information that asciival sends to
# stdout - not the stuff that should be going into the file).

# NODC/OPDB:
set nodb_xb "http://abu.nodc.noaa.gov/bin/nph-jg/nodb_xb"
set nodb_xb_dds ""

set nodb_hctd "http://abu.nodc.noaa.gov/bin/nph-jg/nodb_hctd"
set nodb_hctd_dds ""

set nodb_bo "http://abu.nodc.noaa.gov/bin/nph-jg/nodb_bo"
set nodb_bo_dds ""

# PMEL/COADS:
set coads_pmel "http://ferret.wrc.noaa.gov/cgi-bin/nph-nc/data/coads_climatology.nc"
set coads_pmel_dds "Dataset {
    Float64 COADSX\[COADSX = 180\];
    Float64 COADSY\[COADSY = 90\];
    Float64 TIME\[TIME = 12\];
    Grid {
     ARRAY:
        Float64 SST\[TIME = 12\]\[COADSY = 90\]\[COADSX = 180\];
     MAPS:
        Float64 TIME\[TIME = 12\];
        Float64 COADSY\[COADSY = 90\];
        Float64 COADSX\[COADSX = 180\];
    } SST;
    Grid {
     ARRAY:
        Float64 AIRT\[TIME = 12\]\[COADSY = 90\]\[COADSX = 180\];
     MAPS:
        Float64 TIME\[TIME = 12\];
        Float64 COADSY\[COADSY = 90\];
        Float64 COADSX\[COADSX = 180\];
    } AIRT;
    Grid {
     ARRAY:
        Float64 UWND\[TIME = 12\]\[COADSY = 90\]\[COADSX = 180\];
     MAPS:
        Float64 TIME\[TIME = 12\];
        Float64 COADSY\[COADSY = 90\];
        Float64 COADSX\[COADSX = 180\];
    } UWND;
    Grid {
     ARRAY:
        Float64 VWND\[TIME = 12\]\[COADSY = 90\]\[COADSX = 180\];
     MAPS:
        Float64 TIME\[TIME = 12\];
        Float64 COADSY\[COADSY = 90\];
        Float64 COADSX\[COADSX = 180\];
    } VWND;
} coads_climatology;"

# URI/COADS:
set coads "http://dods.gso.uri.edu/cgi-bin/nph-nc/data/coads_climatology.nc"
set coads_sst_ce "SST\\\[0:0\\\]\\\[10:20\\\]\\\[50:60\\\]"
set coads_dds "Dataset {
    Float64 COADSX\[COADSX = 180\];
    Float64 COADSY\[COADSY = 90\];
    Float64 TIME\[TIME = 12\];
    Grid {
     ARRAY:
        Float64 SST\[TIME = 12\]\[COADSY = 90\]\[COADSX = 180\];
     MAPS:
        Float64 TIME\[TIME = 12\];
        Float64 COADSY\[COADSY = 90\];
        Float64 COADSX\[COADSX = 180\];
    } SST;
    Grid {
     ARRAY:
        Float64 AIRT\[TIME = 12\]\[COADSY = 90\]\[COADSX = 180\];
     MAPS:
        Float64 TIME\[TIME = 12\];
        Float64 COADSY\[COADSY = 90\];
        Float64 COADSX\[COADSX = 180\];
    } AIRT;
    Grid {
     ARRAY:
        Float64 UWND\[TIME = 12\]\[COADSY = 90\]\[COADSX = 180\];
     MAPS:
        Float64 TIME\[TIME = 12\];
        Float64 COADSY\[COADSY = 90\];
        Float64 COADSX\[COADSX = 180\];
    } UWND;
    Grid {
     ARRAY:
        Float64 VWND\[TIME = 12\]\[COADSY = 90\]\[COADSX = 180\];
     MAPS:
        Float64 TIME\[TIME = 12\];
        Float64 COADSY\[COADSY = 90\];
        Float64 COADSX\[COADSX = 180\];
    } VWND;
} coads_climatology;"

# URI/FNOC
set fnoc1 "http://dods.gso.uri.edu/cgi-bin/nph-nc/data/fnoc1.nc"
set fnoc1_ce "u\\\[0:0\\\]\\\[0:9\\\]\\\[0:9\\\]"
set fnoc2 "http://dods.gso.uri.edu/cgi-bin/nph-nc/data/fnoc2.nc"
set fnoc2_ce "u\\\[0:0\\\]\\\[0:9\\\]\\\[0:9\\\]"
set fnoc3 "http://dods.gso.uri.edu/cgi-bin/nph-nc/data/fnoc3.nc"
set fnoc3_ce "u\\\[0:0\\\]\\\[0:9\\\]\\\[0:9\\\],v\\\[0:0\\\]\\\[4:9\\\]\\\[4:9\\\]"

# URI/DSP:
set dsp_1 "http://dods.gso.uri.edu/cgi-bin/nph-dsp/data/f96243170857.img"
set dsp_1_ce "dsp_band_1\\\[20:30\\\]\\\[20:30\\\]"
set dsp_1_dds "Dataset {
    Grid {
     ARRAY:
        Byte dsp_band_1\[lat = 512\]\[lon = 512\];
     MAPS:
        Float64 lat\[lat = 512\];
        Float64 lon\[lon = 512\];
    } dsp_band_1;
    Float64 lat\[lat = 512\];
    Float64 lon\[lon = 512\];
} f96243170857;"

set dsp_2 "http://dods.gso.uri.edu/cgi-bin/nph-dsp/data/east.coast"
set dsp_2_ce "dsp_band_1\\\[20:30\\\]\\\[20:30\\\]"
set dsp_2_dds "Dataset {
    Byte dsp_band_1\[line = 512\]\[pixel = 512\];
} east;"

# URI/MatLab:
set nscat_s2 "http://dods.gso.uri.edu/cgi-bin/nph-dsp/data/NSCAT_S2.mat"
set nscat_s2_dds "Dataset {
    Float64 NSCAT_S2\[NSCAT_S2_row = 153\]\[NSCAT_S2_column = 843\];
} NSCAT_S2;"

# JPL/NSCAT:  (I will get valid filenames from Todd soon)
		
#		http://dods.jpl.nasa.gov/cgi-bin/hdf/data/nscat/???

# JGOFS:
set jg_diatoms "http://dods.gso.uri.edu/cgi-bin/nph-jg/diatoms"
set jg_diatoms_dds "Dataset {
    Sequence {
        String sta;
        String event;
        String tow;
        String diatom_Nit_bi;
        String diatom_Nit_oth;
        String diatom_Pseudo;
    } Level_0;
} diatoms;"

set jg_ctd "http://dods.gso.uri.edu/cgi-bin/nph-jg/ctd"
set jg_ctd_dds "Dataset {
    Sequence {
        String sta;
        String cast;
        String event;
        String year;
        String mon;
        String day;
        String time;
        String lat;
        String lon;
        Sequence {
            String depth;
            String press;
            String temp;
            String cond;
            String sal;
            String potemp;
            String sig_th;
            String sig_t;
            String fluor;
            String beam_cp;
            String par;
        } Level_1;
    } Level_0;
} ctd;"

set jg_bot "http://dods.gso.uri.edu/cgi-bin/nph-jg/bot"
set jg_bot_dds "Dataset {
    Sequence {
        String event;
        String cast;
        Sequence {
            String qflag;
            String sta;
            String bot;
            String depth;
            String press;
            String pressbin;
            String ctdtemp;
            String ctdsal;
            String ctdsig_t;
            String botox;
            String botsal;
            String ctdpotemp;
            String ctdsig_th;
        } Level_1;
    } Level_0;
} bot;"

set jg_ctd_ce "sta,time"

set hdf_winds  "http://dods.jpl.nasa.gov/dods-bin/nph-hdf/pub/ocean_wind/nscat/data/L30/S2/S3096259.HDF.Z"
set hdf_winds_ce "Avg_Wind_Speed\\\[0:5\\\],RMS_Wind_Speed\\\[0:5\\\]"

set stdin_test_data  "testsuite/asciival.1/stdin_test"

