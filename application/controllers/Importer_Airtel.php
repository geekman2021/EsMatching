<?php
    require 'vendor/autoload.php';

    use PhpOffice\PhpSpreadsheet\Spreadsheet;
    use PhpOffice\PhpSpreadsheet\Writer\Xlsx;
 class Importer_Airtel extends CI_Controller {

        public function __construct(){
            parent::__construct();
            $this->load->model("igor_airtel_model");
            $this->load->model("igor_airtel_anomalie_model");
            $this->load->model("airtel_anomalie_model");
            $this->load->model("historique_airtel_model");
            session_start();
        }

        public function index() {
            $this->load->view("templates/sidebar");
            $this->load->view("pages/importer/importer");
            $this->load->view("pages/operateur/airtel-form");

            $_SESSION["last_solde"] = $this->historique_airtel_model->get_last_solde();

                $_SESSION["igor_anomalie_ci"] = array(
                array(
                    "COMPTE" => 9194100008, "DATE_OPER"=> "08-09-2023", "DATE_VAL" => "11.09.2023", "DEVISE" =>"MGA", "MONTANT" =>160000, "LIBELLE" => "Annul BOA to AIRTEL (TEL: 261330503981 MGA:160000 DATE: 0218104353 #null)   ", "OPER" => "CASHI" , "EXPL"=> "AU", "REF_IGOR"=>"SIA3507"
                ),
                array(
                    "COMPTE" => 9194100008, "DATE_OPER"=> "08-09-2023", "DATE_VAL" => "11.09.2023", "DEVISE" =>"MGA", "MONTANT" =>160000, "LIBELLE" => "Annul BOA to AIRTEL (TEL: 261330503981 MGA:160000 DATE: 0218104353 #null)   ", "OPER" => "CASHI" , "EXPL"=> "AU", "REF_IGOR"=>"SIA3508"
                ),
                array(
                    "COMPTE" => 9194100008, "DATE_OPER"=> "08-09-2023", "DATE_VAL" => "11.09.2023", "DEVISE" =>"MGA", "MONTANT" =>160000, "LIBELLE" => "Annul BOA to AIRTEL (TEL: 261330503981 MGA:160000 DATE: 0218104353 #null)   ", "OPER" => "CASHI" , "EXPL"=> "AU", "REF_IGOR"=>"SIA3509"
                    )
                );

                $_SESSION["deallocation"] = array(
                    array("transfer_id" => "DL230908.1123.C00005",	"transfer_date" => "08-SEP-23", "external_id" => "",	"account_no" => "111111111111111", "sender_msisdn" =>	"fd7afe40fffd7c56d838260a9444c722607ae6063d153ad2", "dest_msisdn" =>	"334680675", "amount" =>500000000, "description" =>	"Transaction Success", "service_name" => "Deallocation Transfer",	"reference_number" => "MBANKING080929"),
                    array("transfer_id" => "DL230908.1123.C00006",	"transfer_date" => "08-SEP-23", "external_id" => "",	"account_no" => "111111111111111", "sender_msisdn" =>	"fd7afe40fffd7c56d838260a9444c722607ae6063d153ad2", "dest_msisdn" =>	"330202415", "amount" =>500000000, "description" =>	"Transaction Success", "service_name" =>	"Deallocation Transfer",	"reference_number" => "MBANKING080929"),
                    array("transfer_id" => "DL230908.1123.C00007",	"transfer_date" => "08-SEP-23", "external_id" => "",	"account_no" => "111111111111111", "sender_msisdn" =>	"fd7afe40fffd7c56d838260a9444c722607ae6063d153ad2", "dest_msisdn" =>	"332492434", "amount" =>300000000, "description" =>	"Transaction Success", "service_name" =>	"Deallocation Transfer",	"reference_number" => "MBANKING080929"),
                    array("transfer_id" => "DL230908.1123.C00007",	"transfer_date" => "08-SEP-23", "external_id" => "",	"account_no" => "111111111111111", "sender_msisdn" =>	"fd7afe40fffd7c56d838260a9444c722607ae6063d153ad2", "dest_msisdn" =>	"332492434", "amount" =>500000000, "description" =>	"Transaction Success", "service_name" =>	"Deallocation Transfer",	"reference_number" => "MBANKING0830"),
                    
                );

                $_SESSION["anomalie_vi"]= array(
                    array(
                        "COMPTE" => 9194100008, "DATE_OPER"=> "08-09-2023", "DATE_VAL" => "11.09.2023", "DEVISE" =>"MGA", "MONTANT" =>1300000000, "LIBELLE" => "DEALLOCATION MBANKING080923 MGA 1 300 000 000", "OPER" => "VI" , "EXPL"=> "ARM", "REF_IGOR"=>"TUM9039"
                    )
                );

                


            
        }

        public function importer() {

            // $upload_file_airtel= $_FILES["airtel"]["name"];
            // $upload_file_igor= $_FILES["airtel"]["name"];
            // $extension_airtel = pathinfo($upload_file_airtel, PATHINFO_EXTENSION);
            // $extension_igor = pathinfo($upload_file_igor, PATHINFO_EXTENSION);
            $airtel_test = array(
                array("transfer_id" => "DL230908.1123.C00005",	"transfer_date" => "08-SEP-23", "external_id" => "",	"account_no" => "111111111111111", "sender_msisdn" =>	"fd7afe40fffd7c56d838260a9444c722607ae6063d153ad2", "dest_msisdn" =>	"334680675", "amount" =>500000000, "description" =>	"Transaction Success", "service_name" => "Deallocation Transfer",	"reference_number" => "MBANKING080929"),
                array("transfer_id" => "DL230908.1123.C00006",	"transfer_date" => "08-SEP-23", "external_id" => "",	"account_no" => "111111111111111", "sender_msisdn" =>	"fd7afe40fffd7c56d838260a9444c722607ae6063d153ad2", "dest_msisdn" =>	"330202415", "amount" =>500000000, "description" =>	"Transaction Success", "service_name" =>	"Deallocation Transfer",	"reference_number" => "MBANKING080929"),
                array("transfer_id" => "DL230908.1123.C00007",	"transfer_date" => "08-SEP-23", "external_id" => "",	"account_no" => "111111111111111", "sender_msisdn" =>	"fd7afe40fffd7c56d838260a9444c722607ae6063d153ad2", "dest_msisdn" =>	"332492434", "amount" =>500000000, "description" =>	"Transaction Success", "service_name" =>	"Deallocation Transfer",	"reference_number" => "MBANKING080929"),
                array("transfer_id" => "DL230908.1123.C00007",	"transfer_date" => "08-SEP-23", "external_id" => "",	"account_no" => "111111111111111", "sender_msisdn" =>	"fd7afe40fffd7c56d838260a9444c722607ae6063d153ad2", "dest_msisdn" =>	"332492434", "amount" =>500000000, "description" =>	"Transaction Success", "service_name" =>	"Deallocation Transfer",	"reference_number" => "MBANKING080930"),
                
            );

           
            
            $igor_test= array(
                array(
                    "COMPTE" => 9194100008, "DATE_OPER"=> "08-09-2023", "DATE_VAL" => "11.09.2023", "DEVISE" =>"MGA", "MONTANT" =>1300000000, "LIBELLE" => "DEALLOCATION MBANKING080923 MGA 1 300 000 000", "OPER" => "VI" , "EXPL"=> "ARM", "REF_IGOR"=>"TUM9039"
                ),
                array(
                    "COMPTE" => 9194100008, "DATE_OPER"=> "08-09-2023", "DATE_VAL" => "11.09.2023", "DEVISE" =>"MGA", "MONTANT" =>160000, "LIBELLE" => "Annul BOA to AIRTEL (TEL: 261330503981 MGA:160000 DATE: 0218104353 #null)   ", "OPER" => "CASHI" , "EXPL"=> "W5", "REF_IGOR"=>"SIA3507"
                ),
                array(
                    "COMPTE" => 9194100008, "DATE_OPER"=> "08-09-2023", "DATE_VAL" => "11.09.2023", "DEVISE" =>"MGA", "MONTANT" =>160000, "LIBELLE" => "Annul BOA to AIRTEL (TEL: 261330503981 MGA:160000 DATE: 0218104353 #null)   ", "OPER" => "CASHI" , "EXPL"=> "W5", "REF_IGOR"=>"SIA3508"
                ),
                array(
                    "COMPTE" => 9194100008, "DATE_OPER"=> "08-09-2023", "DATE_VAL" => "11.09.2023", "DEVISE" =>"MGA", "MONTANT" =>160000, "LIBELLE" => "Annul BOA to AIRTEL (TEL: 261330503981 MGA:160000 DATE: 0218104353 #null)   ", "OPER" => "CASHI" , "EXPL"=> "W5", "REF_IGOR"=>"SIA3509"
                )
            );

            

            $airtel_read = $this->readData($_FILES["airtel"]["name"], $_FILES["airtel"]["tmp_name"]);
            $igor_read= $this->readData($_FILES["igor"]["name"], $_FILES["igor"]["tmp_name"]);
            $airtel= array();
            $igor= array();
            if (count($airtel_read) > 1 ) {
                
                for($i=1; $i < count($airtel_read); $i++) {

                    // $date= $airtel_read[$i][2];
                    // $date = DateTime::createFromFormat('d-M-Y', $date);
                    // $date = $date->format('d/m/Y');
                    $airtel[]= array(
                       "TRANSFER_ID" => $airtel_read[$i][0],
                       "transfer_date" =>  $airtel_read[$i][1],
                       "external_id" => $airtel_read[$i][2],
                       "account_no" => $airtel_read[$i][3],
                       "sender_msisdn" => $airtel_read[$i][4],
                       "dest_msisdn" => $airtel_read[$i][5],
                       "amount" => $airtel_read[$i][6],
                       "description" => $airtel_read[$i][7],
                       "service_name" => $airtel_read[$i][8],
                       "reference_number" => $airtel_read[$i][9]
                    );
                }
            }

            if (count($igor_read) > 1 ) {
                
                for($i=1; $i < count($igor_read); $i++) {

                    // $date= $airtel_read[$i][2];
                    // $date = DateTime::createFromFormat('d-M-Y', $date);
                    // $date = $date->format('d/m/Y');
                    $igor[]= array(
                       "COMPTE" => $igor_read[$i][0],
                       "DATE_OPER" =>  $igor_read[$i][1],
                       "DATE_VAL" => $igor_read[$i][2],
                       "DEVISE" => $igor_read[$i][3],
                       "MONTANT" => $igor_read[$i][4],
                       "LIBELLE" => $igor_read[$i][5],
                       "OPER" => $igor_read[$i][6],
                       "EXPL" => $igor_read[$i][7],
                       "REF_IGOR" => $igor_read[$i][8],
                    );
                }
            }


// --------------------------------------------------------------------------------AIRTEL ---------------------------------------------------------------------


            $airtel = $this->supprimerEspace($airtel);
            $deallocation = $this->filtrerDeallocation($airtel);
            $rollback= $this->filtrerRollback($airtel);
            $ambiguous= $this->filtrerAmbiguous($airtel);

            $airtelCI= $this->filtrerAirtelCashIn($airtel);
            $airtelCI = $this->trierParExternalId($airtelCI);

            $airtelCO= $this->filtrerAirtelCashOut($airtel);
            $airtelCO= $this->trierParExternalId($airtelCO);

// -------------------------------------------------------------------------------IGOR------------------------------------------------------------

            $igor = $this->supprimerEspace($igor);
            $resultat= $this->filtrerIgorCIEtNonAu($igor);

            $igorCI= $resultat[0];
            $igorVI= $resultat[1];
            $igorRegul= $resultat[2];
            $igorCI= $this->trierParRefIgor($igorCI);

            $igorCO= $this->filtrerIgorCO($igor);
            $igorCO= $this->trierParRefIgor($igorCO);

            
           
//


// -----------------------------------------------------------------------------COMPARAISON CI -------------------------------------------------------

            $res= $this->comparerRollBackEtAmbigous($rollback, $ambiguous);
            $airtelIgorNormaleCI = $this->comparerAirtelEtIgorCI($airtelCI, $igorCI);
            $resultatAnomalieCI= $this->filtrerAnomalieCI($airtelCI, $igorCI);
            $anomalieAirtelCI= $resultatAnomalieCI[0];
            $anomalieIgorCI= $resultatAnomalieCI[1];
            $dernierRollBack = $res[0];
            $dernierAmbiguous = $res[1];




// ------------------------------------------------------------------------------ COMPARAISON CO ---------------------------------------------------------

            $airtelIgorNormaleCO = $this->comparerAirtelEtIgorCI($airtelCO, $igorCO);
            $resultatAnomalieCO= $this->filtrerAnomalieCI($airtelCO, $igorCO);

            $anomalieAirtelCO= $resultatAnomalieCO[0];
            $anomalieIgorCO = $resultatAnomalieCO[1];

            

// ------------------------------------------------------------------REGULARISATION-------------------------------------------------------------------------------------
            $nonAU= array(
                array("COMPTE" => 9194100008, "DATE_OPER"=> "08-09-2023", "DATE_VAL" => "11.09.2023", "DEVISE" =>"MGA", "MONTANT" =>160000, "LIBELLE" => "Annul BOA to AIRTEL (TEL: 261330503981 MGA:160000 DATE: 0218104353 #null)   ", "OPER" => "CASHI" , "EXPL"=> "W5", "REF_IGOR"=>"SIA3507"),
                array("COMPTE" => 9194100008, "DATE_OPER"=> "08-09-2023", "DATE_VAL" => "11.09.2023", "DEVISE" =>"MGA", "MONTANT" =>160000, "LIBELLE" => "Annul BOA to AIRTEL (TEL: 261330503981 MGA:160000 DATE: 0218104353 #null)   ", "OPER" => "CASHI" , "EXPL"=> "W5", "REF_IGOR"=>"SIA3508")
            );

            
            // echo "--------------------------------------------------";
            // echo "<pre>";
            //     print_r($anomalieIgorCO);
            // echo "</pre>";

            


                
                



            // $this->igor_airtel_model->insert_or_update_co($airtelIgorNormaleCO);

                

            // $this->RegulViDeallocation($_SESSION["anomalie_vi"], $_SESSION["deallocation"]);
            // $this->exporter($airtelIgorNormaleCI, $airtelIgorNormaleCO, $dernierAmbiguous, $dernierRollBack, $anomalieAirtelCI, $anomalieAirtelCO, $anomalieIgorCI, $anomalieIgorCO, $igorVI, $deallocation);

       
            // $this->RegulNonAu($nonAU, $_SESSION["igor_anomalie_ci"]);

// ------------------------------------------------------------- INSERTION ------------------------------------------------------------------------------------

            // foreach($airtelIgorNormaleCO as $item) {
            //     $this->igor_airtel_model->insert_or_update_co($item);
            //     $solde_airtel+=$item["solde"];
            //     $solde_boa+= $item["MONTANT"];
            //     $item["solde_airtel"]=  $solde_airtel;
            //     $item["solde_boa"]= $solde_boa;
            //     $this->historique_airtel_model->insert($item);
            // }

            // foreach($airtelIgorNormaleCI as $item) {
            //     $this->igor_airtel_model->insert_or_update_ci($item);
            //     $solde_airtel+=$item["solde"];
            //     $solde_boa+= $item["MONTANT"];
            //     $item["solde_airtel"]=  $solde_airtel;
            //     $item["solde_boa"]= $solde_boa;
            //     $this->historique_airtel_model->insert($item);
            // }

            // foreach($anomalieIgorCO as $item) {
            //     $this->igor_airtel_anomalie_model->insert_or_update_co($item);
            //     $solde_airtel+=$item["solde"];
            //     $solde_boa+= $item["MONTANT"];
            //     $item["solde_airtel"]=  $solde_airtel;
            //     $item["solde_boa"]= $solde_boa;
            //     $this->historique_airtel_model->insert($item);
            // }

            // foreach($anomalieIgorCI as $item) {
            //     $this->igor_airtel_anomalie_model->insert_or_update_ci($item);
            //     $solde_airtel+=$item["solde"];
            //     $solde_boa+= $item["MONTANT"];
            //     $item["solde_airtel"]=  $solde_airtel;
            //     $item["solde_boa"]= $solde_boa;
            //     $this->historique_airtel_model->insert($item);

            // }

            // foreach ($anomalieAirtelCI as $item ) {
            //     $this->airtel_anomalie_model->insert_or_update_ci($item);
            //     $solde_airtel+=$item["solde"];
            //     $solde_boa+= $item["MONTANT"];
            //     $item["solde_airtel"]=  $solde_airtel;
            //     $item["solde_boa"]= $solde_boa;
            //     $this->historique_airtel_model->insert($item);
            // }

            // foreach($anomalieAirtelCO as $item) {
            //     $this->airtel_anomalie_model->insert_or_update_co($item);
            //     $solde_airtel+=$item["solde"];
            //     $solde_boa+= $item["MONTANT"];
            //     $item["solde_airtel"]=  $solde_airtel;
            //     $item["solde_boa"]= $solde_boa;
            //     $this->historique_airtel_model->insert($item);
            // }

            // foreach($ambiguous as $item) {
            //     $this->airtel_anomalie_model->insert_or_update_ambiguous($item);
            // }

            // foreach($rollback as $item) {
            //     $this->airtel_anomalie_model->insert_or_update_rb($item);
            // }

            // foreach($igorVI as $item) {
            //     $this->igor_airtel_anomalie_model->insert_or_update_vi($item);
            // }

            // foreach($airtelIgorNormaleCO as $item) {
            //     $this->igor_airtel_model->insert_or_update_co($item);
            // }

            // foreach($airtelIgorNormaleCI as $item) {
            //     $this->igor_airtel_model->insert_or_update_ci($item);
            // }

            $historique = array_merge($airtelIgorNormaleCI, $airtelIgorNormaleCO, $igorVI, $dernierAmbiguous, $dernierRollBack);
            
            if(count($_SESSION["last_solde"]) > 0 ) {
                $solde_airtel= $_SESSION["last_solde"][0]->solde_airtel;
                $solde_boa= $_SESSION["last_solde"][0]->solde_boa;
            } else if(count($_SESSION["last_solde"]) === 0 ) {
                $solde_airtel= 0;
                $solde_boa= 0;
            }

            foreach ($historique as $item) {
                $solde_airtel += isset($item["solde"]) ? $item["solde"] : 0;
                $solde_boa += isset($item["MONTANT"]) ? $item["MONTANT"] : 0;
                $item["solde_airtel"] = $solde_airtel;
                $item["solde_boa"] = $solde_boa;
                $this->historique_airtel_model->insert($item);
            }

            



           

            session_destroy();
            // foreach()
}






        
    



// --------------------------------------------------------------------------------- INSERTION -------------------------------------------------------------


                   
            
            
            // echo "--------------------------------------------------";
            // echo "<pre>";
            //     print_r($igor_test);
            // echo "</pre>";
            // echo "--------------------------------------------------";
            


            // if($extension_airtel ==="csv") {
            //     $reader_airtel= new \PhpOffice\PhpSpreadsheet\Reader\Csv();

            // } else if ($extension_airtel =="xls") {
            //     $reader_airtel= new \PhpOffice\PhpSpreadsheet\Reader\Xls();
            // } else {
            //     $reader_airtel=  new \PhpOffice\PhpSpreadsheet\Reader\Xlsx();
            // }

            // if($extension_igor ==="csv") {
            //     $reader_igor= new \PhpOffice\PhpSpreadsheet\Reader\Csv();

            // } else if ($extension_igor =="xls") {
            //     $reader_igor= new \PhpOffice\PhpSpreadsheet\Reader\Xls();
            // } else {
            //     $reader_igor=  new \PhpOffice\PhpSpreadsheet\Reader\Xlsx();
            // }

            // $spreadsheet= $reader_airtel->load($_FILES["airtel"]["tmp_name"]);
            // $sheetdata= $spreadsheet->getActiveSheet()->toArray();

            // // $spreadsheet= $reader->load($_FILES["airtel"]["tmp_name"]);
            // // $sheetdata= $spreadsheet->getActiveSheet()->toArray();

            // // $spreadsheet= $reader->load($_FILES["airtel"]["tmp_name"]);
            // // $sheetdata= $spreadsheet->getActiveSheet()->toArray();
            // echo "<pre>";
            //     print_r($sheetdata);
            // echo "</pre>";

        

        public function readData($upload_file, $upload_tmp) {
            $extension = pathinfo($upload_file, PATHINFO_EXTENSION);

            if($extension ==="csv") {
                $reader= new \PhpOffice\PhpSpreadsheet\Reader\Csv();

            } else if ($extension =="xls") {
                $reader= new \PhpOffice\PhpSpreadsheet\Reader\Xls();
            } else {
                $reader=  new \PhpOffice\PhpSpreadsheet\Reader\Xlsx();
            }

            $spreadsheet= $reader->load($upload_tmp);
            $sheetdata= $spreadsheet->getActiveSheet()->toArray();

            return $sheetdata;

        }

        public function supprimerEspace($data) {
            $resultat= array();
            foreach($data as $key=> $valeur) {
                $resultat[$key] = str_replace(" ", "", $valeur);
            }

            return $resultat;
        }

        public function filtrerDeallocation ($data) {
            $resultat= array();
            foreach($data as $item) {
                if($item["service_name"] ==="DeallocationTransfer") {
                    $item["solde"] = $item["amount"] * -1;
                    $resultat[]= $item;
                }
            }
            return $resultat;
        }

        public function filtrerAirtelCashIn($data) {
            $reultat= array();

            foreach($data as $item) {
                if(!empty($item["external_id"]) && ($item["service_name"]==="BanktoWalletTransfer" || $item["service_name"] === "ChannelBanktoWalletTransfer" )) {
                    $item["solde"] = $item["amount"] * -1;
                    $resultat[]= $item;
                }
            }

            return $resultat;
        }

        public function filtrerAirtelCashOut($data) {
            $resultat= array();
            foreach($data as $item) {
                if(!empty($item["external_id"]) && ($item["service_name"]==="ChannelWalletToBankTransfer" || $item["service_name"] === "WalletToBankTransfer" || $item["service_name"] === "AutoSweepMoneyTransfer" )) {
                    $item["solde"] = $item["amount"] * 1;
                    $resultat[]= $item;
                }
            }
            return $resultat;
        }

        public function filtrerRollback($data) {
            $resultat= array();

            foreach($data as $item) {
                if ($item["service_name"] ==="RollBack") {
                    $item["solde"] = $item["amount"] * (-1);
                    $resultat[]= $item;
                }
            } 

            return $resultat;
        }

        public function filtrerAmbiguous($data) {
            $resultat= array();

            foreach($data as $item) {
                if($item["description"] ==="TransactionAmbiguous" || ($item["description"] === "TransactionSuccess" && empty($item["external_id"]))  && ($item["service_name"] === "BanktoWalletTransfer" || $item["service_name"] === "ChannelBanktoWalletTransfer") ) {
                    $item["solde"] = $item["amount"] * (-1);
                    $resultat[] = $item;
                }
            }
            return $resultat;
        }

        public function comparerRollBackEtAmbigous($rollback, $ambiguous) {
            $dernierRollBack = array();
            $dernierAmbiguous = array();

            $rollbackHash = array();
            foreach ($rollback as $itemRoll) {
                $rollbackHash[$itemRoll["reference_number"]] = true;
            }
            foreach ($ambiguous as $itemAmbi) {
                if (!isset($rollbackHash[$itemAmbi["TRANSFER_ID"]])) {
                    $dernierAmbiguous[] = $itemAmbi;
                }
            }
        
            return [$dernierRollBack, $dernierAmbiguous];
        }
        

        public function trierParRefIgor($data) {
            usort($data, function($a, $b) {
                return strcmp($a['REF_IGOR'], $b['REF_IGOR']);
            });

            return $data;
        }

        public function trierParExternalId($data) {
            usort($data, function($a, $b) {
                return strcmp($a['external_id'], $b['external_id']);
            });

            return $data;
        }

        public function comparerAirtelEtIgorCI($airtel, $igor) {
            $mergedAirtelEtIgor= array();
            $normaleIgor= array();
            $normaleAirtel= array();

            foreach ($airtel as $itemAirtel) {
                foreach($igor as $itemIgor) {
                    if($itemAirtel["external_id"] === $itemIgor["REF_IGOR"]) {
                        $normaleAirtel[]= $itemAirtel;
                    } 
                }
            }

            foreach ($igor as $itemIgor) {
                foreach($airtel as $itemAirtel) {
                    if($itemAirtel["external_id"] === $itemIgor["REF_IGOR"]) {
                        $normaleIgor[]= $itemIgor;
                    }
                }
            }

            
            for ($i=0; $i < count($normaleAirtel) ; $i++) {
                 array_push($mergedAirtelEtIgor, array_merge($normaleAirtel[$i], $normaleIgor[$i])) ;
            }
            return $mergedAirtelEtIgor;
        }

        public function filtrerAnomalieCI($airtel, $igor) {
            $externalIdsIgor = array_column($igor, 'REF_IGOR');
            $externalIdsAirtel = array_column($airtel, 'external_id');
        
            $anomalieAirtel = array_filter($airtel, function ($item) use ($externalIdsIgor) {
                return !in_array($item['external_id'], $externalIdsIgor);
            });
        
            $anomalieIgor = array_filter($igor, function ($item) use ($externalIdsAirtel) {
                return !in_array($item['REF_IGOR'], $externalIdsAirtel);
            });
        
            return [$anomalieAirtel, $anomalieIgor];
        }

        public function filtrerIgorCIEtNonAu($data) {
            $igorCI= array();
            $igorVI= array();
            $igorRegul = array();

            foreach($data as $item) {
                if($item["OPER"]==="CASHI" && $item["EXPL"] ==="AU") {
                    $igorCI[] = $item;
                } else if ($item["EXPL"] !=="AU" && $item["OPER"] ==="VI") {
                    $igorVI[] = $item;
                } else if ($item["EXPL"] != "AU" && $item["OPER"] ==="CASHI") {
                    $igorRegul[]= $item;
                }
            }
            return [$igorCI, $igorVI, $igorRegul];
        }

        public function filtrerIgorCO ($data) {
            $resultat= array();

            foreach($data as $item) {
                if($item["OPER"] ==="CASHO" && $item["EXPL"] ==="AU") {
                    $resultat[]= $item;
                }
            }

            return $resultat;
        }


        public function RegulViDeallocation($vi, $deallocation) {
            $viCopy= $vi;
            $deallocationCopy= $deallocation;

            $reference_number_to_total_amount = array();

            foreach ($deallocation as $itemDeallo) {
                $reference_number = $itemDeallo['reference_number'];
                $amount = $itemDeallo['amount'];
                
                if (array_key_exists($reference_number, $reference_number_to_total_amount)) {
                    $reference_number_to_total_amount[$reference_number] += $amount;
                } else {
                    $reference_number_to_total_amount[$reference_number] = $amount;
                }
            }

            foreach ($reference_number_to_total_amount as $reference_number => $total_amount) {
                foreach($vi as $itemVi) {
                    // echo "------------MANDALO ----------------" .$itemVi["MONTANT"] .$total_amount ;
                    if($itemVi["MONTANT"] === $total_amount) {
                        
                        for ($i=0; $i < count($deallocationCopy); $i++) {
                            if($deallocationCopy[$i]["reference_number"] === $reference_number) {
                                $deallocationCopy[$i]["ref_igor"] = $itemVi["REF_IGOR"];
                            }
                        }
                    }
                }
                // echo "Total amount for reference number $reference_number: $total_amount\n";
            }

            // echo "<pre>";
            //     print_r($deallocationCopy);
            // echo "</pre>";
            session_destroy();
        } 
        
        public function RegulNonAu($nonAu, $anomalieIgor) {

            $regul= array();
            foreach($nonAu as $itemNonAU) {
                foreach($anomalieIgor as $itemAno) {
                    if($itemAno["REF_IGOR"] ===$itemNonAU["REF_IGOR"]) {
                        $regul[]= $itemNonAU;
                        $regul[]= $itemAno;
                    }
                }
            }

            // echo "<pre>";
            //     print_r($regul);
            // echo "</pre>";

            return $regul;
        }

        public function regulRollBackAmbiguous($rollback, $ambiguous) {

            $regulRoll= array();
            $regulAmbi= array();
            foreach($rollback as $itemRoll) {
                foreach($ambiguous as $itemAmbi) {
                    if($itemAmbi["TRANSFER_ID"]===$itemRoll["reference_number"]) {
                        $regulRoll[] = $itemRoll;
                        $regulAmbi[] = $itemAmbi;
                    }
                }
            }
        }



        public function exporter($airtelIgorNormaleCI, $airtelIgorNormaleCO, $dernierAmbiguous, $dernierRollBack, $anomalieAirtelCI, $anomalieAirtelCO, $anomalieIgorCI, $anomalieIgorCO, $igorVI, $deallocation) {

            $dateAujourdhui = date("Y-m-d");
            $nomFichier = "RapproAirtel-" .$dateAujourdhui .".xlsx";

            header("Content-Type: application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");
            header("Content-Disposition: attachment; filename=\"" . $nomFichier . "\"");
            
            $spreadsheet = new PhpOffice\PhpSpreadsheet\Spreadsheet();
            $sheet = $spreadsheet->getActiveSheet();

            $cell_array = ["A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X"];
            $header = ["TRI", "DATE OPE", "DATE VAL", "DEVISE", "MONTANT", "LIBELLE", "CODE OPER", "EXPL", "REF", "SOLDE", " ", "TRANSFER_ID", "DATE OPE", "REF", "ACCOUNT_NO", "SENDER MSISDN", "DESTMSISDN", "MONTANT", "DESCRIPTION", "SERVICE_NAME", "REFERENCE_NUMBER", "MONTANT", "SOLDE", "PRINCIPALE"];

            // echo count($cell_array);

            for ($i = 0; $i < count($cell_array); $i++) {
                $sheet->getStyle($cell_array[$i] . "2")->getFont()->setBold(true);
                $sheet->setCellValue($cell_array[$i] . "2", $header[$i]);

                $sheet->getColumnDimension($cell_array[$i])->setAutoSize(true);
                $sheet->getStyle($cell_array[$i] . "2")->getAlignment()->setHorizontal(\PhpOffice\PhpSpreadsheet\Style\Alignment::HORIZONTAL_CENTER);
                $sheet->getStyle($cell_array[$i] . "3")->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID);
                $sheet->getStyle($cell_array[$i] . "3")->getFill()->getStartColor()->setARGB('FFFF00');
                $sheet->getStyle($cell_array[$i] . "1")->getBorders()->getAllBorders()->setBorderStyle(\PhpOffice\PhpSpreadsheet\Style\Border::BORDER_THIN);
                $sheet->getStyle($cell_array[$i] . "2")->getBorders()->getAllBorders()->setBorderStyle(\PhpOffice\PhpSpreadsheet\Style\Border::BORDER_THIN);

            }

            // $lastRow = $sheet->getHighestRow() + 1;
            // $columnOrder= ["DATE_OPER", "DATE_VAL", "DEVISE", "MONTANT", "LIBELLE", "OPER", "EXPL", "REF_IGOR", "solde", "transfer_date", "external_id", "sender_msisdn", "dest_msisdn", "amount", "description", "service_name", "reference_number", "solde"];


            // foreach ($airtelIgorNormaleCI as $dataRow) {
            //     foreach ($columnOrder as $columnKey) {
            //         if (isset($dataRow[$columnKey])) {
            //             $sheet->setCellValue($cell_array[$colIndex] . $lastRow, $dataRow[$columnKey]);
            //         }
            //     }
            //     $lastRow++;
            // }

            $lastRow = $sheet->getHighestRow() + 1;
            foreach ($airtelIgorNormaleCI as $dataRow) {
                $sheet->setCellValue($cell_array[1] .$lastRow, $dataRow["DATE_OPER"]);
                $sheet->setCellValue($cell_array[2] .$lastRow, $dataRow["DATE_VAL"]);
                $sheet->setCellValue($cell_array[3] .$lastRow, $dataRow["DEVISE"]);
                $sheet->setCellValue($cell_array[4] .$lastRow, $dataRow["MONTANT"]);
                $sheet->setCellValue($cell_array[5] .$lastRow, $dataRow["LIBELLE"]);
                $sheet->setCellValue($cell_array[6] .$lastRow, $dataRow["OPER"]);
                $sheet->setCellValue($cell_array[7] .$lastRow, $dataRow["EXPL"]);
                $sheet->setCellValue($cell_array[8] .$lastRow, $dataRow["REF_IGOR"]);
                $sheet->setCellValue($cell_array[11] .$lastRow, $dataRow["TRANSFER_ID"]);
                $sheet->setCellValue($cell_array[12] .$lastRow, $dataRow["transfer_date"]);
                $sheet->setCellValue($cell_array[13] .$lastRow, $dataRow["external_id"]);
                $sheet->setCellValue($cell_array[14] .$lastRow, $dataRow["account_no"]);
                $sheet->setCellValue($cell_array[15] .$lastRow, $dataRow["sender_msisdn"]);
                $sheet->setCellValue($cell_array[16] .$lastRow, $dataRow["dest_msisdn"]);
                $sheet->setCellValue($cell_array[17] .$lastRow, $dataRow["amount"]);
                $sheet->setCellValue($cell_array[18] .$lastRow, $dataRow["description"]);
                $sheet->setCellValue($cell_array[19] .$lastRow, $dataRow["service_name"]);
                $sheet->setCellValue($cell_array[20] .$lastRow, $dataRow["reference_number"]);
                $sheet->setCellValue($cell_array[21] .$lastRow, $dataRow["solde"]);
                $lastRow++;
            }



            $lastRow = $sheet->getHighestRow() + 2;
            foreach ($airtelIgorNormaleCO as $dataRow) {
                $sheet->setCellValue($cell_array[1] .$lastRow, $dataRow["DATE_OPER"]);
                $sheet->setCellValue($cell_array[2] .$lastRow, $dataRow["DATE_VAL"]);
                $sheet->setCellValue($cell_array[3] .$lastRow, $dataRow["DEVISE"]);
                $sheet->setCellValue($cell_array[4] .$lastRow, $dataRow["MONTANT"]);
                $sheet->setCellValue($cell_array[5] .$lastRow, $dataRow["LIBELLE"]);
                $sheet->setCellValue($cell_array[6] .$lastRow, $dataRow["OPER"]);
                $sheet->setCellValue($cell_array[7] .$lastRow, $dataRow["EXPL"]);
                $sheet->setCellValue($cell_array[8] .$lastRow, $dataRow["REF_IGOR"]);
                $sheet->setCellValue($cell_array[11] .$lastRow, $dataRow["TRANSFER_ID"]);
                $sheet->setCellValue($cell_array[12] .$lastRow, $dataRow["transfer_date"]);
                $sheet->setCellValue($cell_array[13] .$lastRow, $dataRow["external_id"]);
                $sheet->setCellValue($cell_array[14] .$lastRow, $dataRow["account_no"]);
                $sheet->setCellValue($cell_array[15] .$lastRow, $dataRow["sender_msisdn"]);
                $sheet->setCellValue($cell_array[16] .$lastRow, $dataRow["dest_msisdn"]);
                $sheet->setCellValue($cell_array[17] .$lastRow, $dataRow["amount"]);
                $sheet->setCellValue($cell_array[18] .$lastRow, $dataRow["description"]);
                $sheet->setCellValue($cell_array[19] .$lastRow, $dataRow["service_name"]);
                $sheet->setCellValue($cell_array[20] .$lastRow, $dataRow["reference_number"]);
                $sheet->setCellValue($cell_array[21] .$lastRow, $dataRow["solde"]);
                
                $lastRow++;
            }

            $writer = new PhpOffice\PhpSpreadsheet\Writer\Xlsx($spreadsheet);
            $writer->save('php://output');


            // $lastRow = $sheet->getHighestRow() + 3;
            // foreach($igorVI as $dataRow) {

            //     $sheet->setCellValue($cell_array[1] .$lastRow, $dataRow["DATE_OPER"]);
            //     $sheet->setCellValue($cell_array[2] .$lastRow, $dataRow["DATE_VAL"]);
            //     $sheet->setCellValue($cell_array[3] .$lastRow, $dataRow["DEVISE"]);
            //     $sheet->setCellValue($cell_array[4] .$lastRow, $dataRow["MONTANT"]);
            //     $sheet->setCellValue($cell_array[5] .$lastRow, $dataRow["LIBELLE"]);
            //     $sheet->setCellValue($cell_array[6] .$lastRow, $dataRow["OPER"]);
            //     $sheet->setCellValue($cell_array[7] .$lastRow, $dataRow["EXPL"]);
            //     $sheet->setCellValue($cell_array[8] .$lastRow, $dataRow["REF_IGOR"]);

            // }


            
            // $lastRow = $sheet->getHighestRow() + 2;

            // foreach($anomalieIgorCI as $dataRow) {
            //     $sheet->setCellValue($cell_array[1] .$lastRow, $dataRow["DATE_OPER"]);
            //     $sheet->setCellValue($cell_array[2] .$lastRow, $dataRow["DATE_VAL"]);
            //     $sheet->setCellValue($cell_array[3] .$lastRow, $dataRow["DEVISE"]);
            //     $sheet->setCellValue($cell_array[4] .$lastRow, $dataRow["MONTANT"]);
            //     $sheet->setCellValue($cell_array[5] .$lastRow, $dataRow["LIBELLE"]);
            //     $sheet->setCellValue($cell_array[6] .$lastRow, $dataRow["OPER"]);
            //     $sheet->setCellValue($cell_array[7] .$lastRow, $dataRow["EXPL"]);
            //     $sheet->setCellValue($cell_array[8] .$lastRow, $dataRow["REF_IGOR"]);
            //     // $sheet->getStyle($cell_array[1] . $rowIndex)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID);
            //     // $sheet->getStyle($cell_array[1] . $rowIndex)->getFill()->setStartColor($redFill->getStartColor());
            // }


            // $lastRow = $sheet->getHighestRow() + 2;
            // foreach($anomalieIgorCO as $dataRow) {
            //     $sheet->setCellValue($cell_array[1] .$lastRow, $dataRow["DATE_OPER"]);
            //     $sheet->setCellValue($cell_array[2] .$lastRow, $dataRow["DATE_VAL"]);
            //     $sheet->setCellValue($cell_array[3] .$lastRow, $dataRow["DEVISE"]);
            //     $sheet->setCellValue($cell_array[4] .$lastRow, $dataRow["MONTANT"]);
            //     $sheet->setCellValue($cell_array[5] .$lastRow, $dataRow["LIBELLE"]);
            //     $sheet->setCellValue($cell_array[6] .$lastRow, $dataRow["OPER"]);
            //     $sheet->setCellValue($cell_array[7] .$lastRow, $dataRow["EXPL"]);
            //     $sheet->setCellValue($cell_array[8] .$lastRow, $dataRow["REF_IGOR"]);
            //     // for ($col = 1; $col <= 8; $col++) {
            //     //     // $sheet->getStyle($cell_array[$col] . $rowIndex)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID);
            //     //     // $sheet->getStyle($cell_array[$col] . $rowIndex)->getFill()->setStartColor($redFill->getStartColor());
            //     // }
            // }

            // $lastRow = $sheet->getHighestRow() + 2;

            // foreach($anomalieAirtelCI as $item) {
            //     $sheet->setCellValue($cell_array[11] .$lastRow, $dataRow["TRANSFER_ID"]);
            //     $sheet->setCellValue($cell_array[12] .$lastRow, $dataRow["transfer_date"]);
            //     $sheet->setCellValue($cell_array[13] .$lastRow, $dataRow["external_id"]);
            //     $sheet->setCellValue($cell_array[14] .$lastRow, $dataRow["account_no"]);
            //     $sheet->setCellValue($cell_array[15] .$lastRow, $dataRow["sender_msisdn"]);
            //     $sheet->setCellValue($cell_array[16] .$lastRow, $dataRow["dest_msisdn"]);
            //     $sheet->setCellValue($cell_array[17] .$lastRow, $dataRow["amount"]);
            //     $sheet->setCellValue($cell_array[18] .$lastRow, $dataRow["description"]);
            //     $sheet->setCellValue($cell_array[19] .$lastRow, $dataRow["service_name"]);
            //     $sheet->setCellValue($cell_array[20] .$lastRow, $dataRow["reference_number"]);
            //     $sheet->setCellValue($cell_array[21] .$lastRow, $dataRow["solde"]);
            // }



            // $lastRow = $sheet->getHighestRow() + 2;

            // foreach($anomalieAirtelCO as $dataRow) {
            //     $sheet->setCellValue($cell_array[11] .$lastRow, $dataRow["TRANSFER_ID"]);
            //     $sheet->setCellValue($cell_array[12] .$lastRow, $dataRow["transfer_date"]);
            //     $sheet->setCellValue($cell_array[13] .$lastRow, $dataRow["external_id"]);
            //     $sheet->setCellValue($cell_array[14] .$lastRow, $dataRow["account_no"]);
            //     $sheet->setCellValue($cell_array[15] .$lastRow, $dataRow["sender_msisdn"]);
            //     $sheet->setCellValue($cell_array[16] .$lastRow, $dataRow["dest_msisdn"]);
            //     $sheet->setCellValue($cell_array[17] .$lastRow, $dataRow["amount"]);
            //     $sheet->setCellValue($cell_array[18] .$lastRow, $dataRow["description"]);
            //     $sheet->setCellValue($cell_array[19] .$lastRow, $dataRow["service_name"]);
            //     $sheet->setCellValue($cell_array[20] .$lastRow, $dataRow["reference_number"]);
            //     $sheet->setCellValue($cell_array[21] .$lastRow, $dataRow["solde"]);
            // }

            
            // $lastRow = $sheet->getHighestRow() + 2;
            // foreach($dernierAmbiguous as $dataRow) {
            //     $sheet->setCellValue($cell_array[11] .$lastRow, $dataRow["TRANSFER_ID"]);
            //     $sheet->setCellValue($cell_array[12] .$lastRow, $dataRow["transfer_date"]);
            //     $sheet->setCellValue($cell_array[13] .$lastRow, $dataRow["external_id"]);
            //     $sheet->setCellValue($cell_array[14] .$lastRow, $dataRow["account_no"]);
            //     $sheet->setCellValue($cell_array[15] .$lastRow, $dataRow["sender_msisdn"]);
            //     $sheet->setCellValue($cell_array[16] .$lastRow, $dataRow["dest_msisdn"]);
            //     $sheet->setCellValue($cell_array[17] .$lastRow, $dataRow["amount"]);
            //     $sheet->setCellValue($cell_array[18] .$lastRow, $dataRow["description"]);
            //     $sheet->setCellValue($cell_array[19] .$lastRow, $dataRow["service_name"]);
            //     $sheet->setCellValue($cell_array[20] .$lastRow, $dataRow["reference_number"]);
            //     $sheet->setCellValue($cell_array[21] .$lastRow, $dataRow["solde"]);
            // }

            // $lastRow = $sheet->getHighestRow() + 1;
            // foreach($dernierRollBack as $dataRow) {
            //     $sheet->setCellValue($cell_array[11] .$lastRow, $dataRow["TRANSFER_ID"]);
            //     $sheet->setCellValue($cell_array[12] .$lastRow, $dataRow["transfer_date"]);
            //     $sheet->setCellValue($cell_array[13] .$lastRow, $dataRow["external_id"]);
            //     $sheet->setCellValue($cell_array[14] .$lastRow, $dataRow["account_no"]);
            //     $sheet->setCellValue($cell_array[15] .$lastRow, $dataRow["sender_msisdn"]);
            //     $sheet->setCellValue($cell_array[16] .$lastRow, $dataRow["dest_msisdn"]);
            //     $sheet->setCellValue($cell_array[17] .$lastRow, $dataRow["amount"]);
            //     $sheet->setCellValue($cell_array[18] .$lastRow, $dataRow["description"]);
            //     $sheet->setCellValue($cell_array[19] .$lastRow, $dataRow["service_name"]);
            //     $sheet->setCellValue($cell_array[20] .$lastRow, $dataRow["reference_number"]);
            //     $sheet->setCellValue($cell_array[21] .$lastRow, $dataRow["solde"]);
            // }




            // $lastRow = $sheet->getHighestRow() + 3;
            // foreach($deallocation as $dataRow) {
            //     $sheet->setCellValue($cell_array[11] .$lastRow, $dataRow["TRANSFER_ID"]);
            //     $sheet->setCellValue($cell_array[12] .$lastRow, $dataRow["transfer_date"]);
            //     $sheet->setCellValue($cell_array[13] .$lastRow, $dataRow["external_id"]);
            //     $sheet->setCellValue($cell_array[14] .$lastRow, $dataRow["account_no"]);
            //     $sheet->setCellValue($cell_array[15] .$lastRow, $dataRow["sender_msisdn"]);
            //     $sheet->setCellValue($cell_array[16] .$lastRow, $dataRow["dest_msisdn"]);
            //     $sheet->setCellValue($cell_array[17] .$lastRow, $dataRow["amount"]);
            //     $sheet->setCellValue($cell_array[18] .$lastRow, $dataRow["description"]);
            //     $sheet->setCellValue($cell_array[19] .$lastRow, $dataRow["service_name"]);
            //     $sheet->setCellValue($cell_array[20] .$lastRow, $dataRow["reference_number"]);
            //     $sheet->setCellValue($cell_array[21] .$lastRow, $dataRow["solde"]);
            // }


            // $lastRow = $sheet->getHighestRow() + 1;
            // $range = 'J3:J' . $lastRow;
            // $style = $sheet->getStyle($range);

            // $style->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID);
            // $style->getFill()->getStartColor()->setARGB('FFFF00');

            // $range = 'K3:K' . $lastRow;
            // $style= $sheet->getStyle($range);
            // $style->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID);
            // $style->getFill()->getStartColor()->setARGB('FF00FF00');

            // $range = 'W3:W' . $lastRow;
            // $style= $sheet->getStyle($range);
            // $style->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID);
            // $style->getFill()->getStartColor()->setARGB('FF00FFFF');

            // for ($row = 4; $row <= $lastRow; $row++) {
            //     $formula = "=E$row + J" . ($row - 1);
            //     $sheet->setCellValue("J$row", $formula);
            // }

            // for ($row = 4; $row <= $lastRow; $row++) {
            //     $formula = "=V$row + W" . ($row - 1);
            //     $sheet->setCellValue("W$row", $formula);
            // }

            // for ($row = 4; $row <= $lastRow; $row++) {
            //     $formula = "=E$row + V" . $row;
            //     $sheet->setCellValue("X$row", $formula);
            // }


            // $cell = "X1";
            // $style = $sheet->getStyle($cell);
            // $style->getFont()->getColor()->setARGB('FFFF0000');
            // $lastRow = $sheet->getHighestRow();
            // $formula = "=SUM(X3:X$lastRow)";
            // $sheet->setCellValue("X1", $formula);


            // foreach ($dataRow as $key => $value) {
            //         $sheet->setCellValue($cell_array[$colIndex] . $lastRow, $value);
            //         $colIndex++;
            //     }
            // return $lastDataRow;

            // $writer = new PhpOffice\PhpSpreadsheet\Writer\Xlsx($spreadsheet);
            // $writer->save('php://output');

            
    }

    
}



?>