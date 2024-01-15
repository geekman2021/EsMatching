<?php
    require 'vendor/autoload.php';

    use PhpOffice\PhpSpreadsheet\Spreadsheet;
    use PhpOffice\PhpSpreadsheet\Writer\Xlsx;
    use PhpOffice\PhpSpreadsheet\Writer\Xls;
    use PhpOffice\PhpSpreadsheet\Writer\Csv;
 class Importer_Airtel extends CI_Controller {

        public function __construct(){
            parent::__construct();
            $this->load->model("Igor_Airtel_Model");
            $this->load->model("Igor_Airtel_Anomalie_Model");
            $this->load->model("Airtel_Anomalie_Model");
            $this->load->model("Historique_Airtel_Model");
            $this->load->model("Regul_Igor_Airtel_Model");
            $this->load->model("Non_Au_Model");
            ini_set('memory_limit', '1024M');
            set_time_limit(300);
            session_start();
        }

        public function index() {

            $this->load->view("templates/sidebar");
            $this->load->view("pages/importer/importer");
            $this->load->view("pages/operateur/airtel-form");
            $_SESSION["last_solde"] = $this->Historique_Airtel_Model->get_last_solde();
            $_SESSION["igor_anomalie_ci"] = $this->Igor_Airtel_Anomalie_Model->get_anomalie_ci();
            $_SESSION["igor_anomalie_co"] = $this->Igor_Airtel_Anomalie_Model->get_anomalie_co();
            $_SESSION["anomalie_vi"]= $this->Igor_Airtel_Anomalie_Model->get_vi();
            $_SESSION["deallocation"] = $this->Airtel_Anomalie_Model->get_deallocation();
            $_SESSION["ambigu"]= $this->Airtel_Anomalie_Model->get_ambiguous();
            $_SESSION["deallo"]= $this->Airtel_Anomalie_Model->get_deallocation();

        }

        public function importer() {
            $airtel_read = $this->readData($_FILES["airtel"]["name"], $_FILES["airtel"]["tmp_name"]);
            $igor_read= $this->readData($_FILES["igor"]["name"], $_FILES["igor"]["tmp_name"]);

            $airtel= array();
            $igor= array();
            if (count($airtel_read) > 1 ) {
                
                for($i=1; $i < count($airtel_read); $i++) {

                    $date= $airtel_read[$i][1];
                    $date= date_create_from_format('d-M-y', $date);
                    if ($date !== false) {
                        $date = $date->format('Y-m-d');
                    }
                    $airtel[]= array(
                       "TRANSFER_ID" => $airtel_read[$i][0],
                       "transfer_date" =>  $date,
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
                
                for($i=0; $i < count($igor_read); $i++) {

                    $date_val= $igor_read[$i][2];
                    $date_val = date_create_from_format('d.m.Y', $date_val);
                    
                    $date_oper= $igor_read[$i][1];
                    $date_oper = date_create_from_format('d.m.Y', $date_oper);

                    if ($date_oper !== false) {
                        $date_oper = $date_oper->format('Y-m-d');
                    }
                    if ($date_val !== false) {
                        $date_val = $date_val->format('Y-m-d');
                    }
                    
                    $igor[]= array(
                       "COMPTE" => $igor_read[$i][0],
                       "DATE_OPER" =>  $date_oper,
                       "DATE_VAL" => $date_val,
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
            $mergedAmbigousEtRollback = $this->mergedAmbigousEtRollback($rollback, $ambiguous);

// -------------------------------------------------------------------------------IGOR------------------------------------------------------------

            $igor = $this->supprimerEspace($igor);
            $resultat= $this->filtrerIgorCIEtnonAu($igor);

            $igorCI= $resultat[0];
            $igorVI= $resultat[1];
            $nonAu= $resultat[2];
            $igorCI= $this->trierParRefIgor($igorCI);

            $igorCO= $this->filtrerIgorCO($igor);
            $igorCO= $this->trierParRefIgor($igorCO);


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
            

            $rollback= $this->changerCleRollBack($rollback);
            $ambiguRegul =array();
            if(!empty($_SESSION["ambigu"])) {
                foreach($_SESSION["ambigu"] as $sessionItem) {
                    $ambiguRegul[]= get_object_vars($sessionItem);
                }
            }

// // ------------------------------------------------------------------REGULARISATION-------------------------------------------------------------------------------------



//-----------------------------------------------------------REGULARISATION VI ET DEALLOCATION--------------------------------------------------------

            if(empty($_SESSION["deallo"]) && empty($_SESSION["anomalie_vi"]) && empty($igorVI) && !empty($deallocation)) {
                foreach($deallocation as $item) {
                    $this->Airtel_Anomalie_Model->insert_or_update_deallocation($item);
                }
            } else if (empty($_SESSION["deallo"]) && empty($_SESSION["anomalie_vi"]) && !empty($igorVI) && empty($deallocation)) {
                foreach($igorVI as $item) {
                    $this->Igor_Airtel_Anomalie_Model->insert_or_update_vi($item);
                }
            } else if (!empty($_SESSION["deallo"]) && !empty($_SESSION["anomalie_vi"]) && !empty($igorVI) && empty($deallocation)) {
                $v= array();
                $d= array();
                foreach($_SESSION["anomalie_vi"] as $item) {
                    $v[]= get_object_vars($item);
                }
                foreach($_SESSION["deallo"] as $item) {
                    $d[]= get_object_vars($item);
                }
                $vi_merge= array_merge($v, $igorVI);
                $this->RegulViDeallocation($igorVI, $d);
            } else if (empty($_SESSION["deallo"]) && !empty($_SESSION["anomalie_vi"]) && !empty($igorVI) && !empty($deallocation)) {
                $v= array();
                foreach($_SESSION["anomalie_vi"] as $item) {
                    $v[]= get_object_vars($item);
                }
                $vi_merge= array_merge($v, $igorVI);
                $this->RegulViDeallocation($vi_merge, $deallocation);

            } else if (!empty($_SESSION["deallo"]) && empty($_SESSION["anomalie_vi"]) && !empty($igorVI) && !empty($deallocation)) {
                $d= array();
                foreach($_SESSION["deallo"] as $item) {
                    $d[]= get_object_vars($item);
                }
                $deallo_merge= array_merge($d, $deallocation);
                $this->RegulViDeallocation($igorVI, $deallo_merge);
            } else if (!empty($_SESSION["deallo"]) && !empty($_SESSION["anomalie_vi"]) && empty($igorVI) && !empty($deallocation)) {
                $v= array();
                $d= array();
                foreach($_SESSION["deallo"] as $item) {
                    $d[]= get_object_vars($item);
                }
                foreach($_SESSION["anomalie_vi"] as $item) {
                    $v[]= get_object_vars($item);
                }
                $deallo_merge= array_merge($d, $deallocation);
                $this->RegulViDeallocation($v, $deallo_merge);
            }
            
            else if (empty($_SESSION["deallo"]) && !empty($_SESSION["anomalie_vi"]) && !empty($igorVI) && empty($deallocation)) {
                $v= array();
                foreach($_SESSION["anomalie_vi"] as $item) {
                    $v[]= get_object_vars($item);
                }
                $vi_merge= array_merge($v, $igorVI);
                foreach($vi_merge as $item) {
                    $this->Igor_Airtel_Anomalie_Model->insert_or_update_vi($item);
                }
            } else if(!empty($_SESSION["deallo"]) && empty($_SESSION["anomalie_vi"]) && !empty($igorVI) && empty($deallocation)) {
                $d= array();
                foreach($_SESSION["deallo"] as $item) {
                    $d[]= get_object_vars($item);
                }
                $this->RegulViDeallocation($igorVI, $d);
            } else if (empty($_SESSION["deallo"]) && empty($_SESSION["anomalie_vi"]) && !empty($igorVI) && !empty($deallocation)) {
                $this->RegulViDeallocation($igorVI, $deallocation);
            } else if (empty($_SESSION["deallo"]) && !empty($_SESSION["anomalie_vi"]) && empty($igorVI) && !empty($deallocation)) {
                $v= array();
                foreach($_SESSION["anomalie_vi"] as $item) {
                    $v[]= get_object_vars($item);
                }
                $this->RegulViDeallocation($v, $deallocation);
            
            } else if (!empty($_SESSION["deallo"]) && empty($_SESSION["anomalie_vi"]) && empty($igorVI) && !empty($deallocation)) {
                $d= array();
                foreach ($_SESSION["deallo"] as $item) {
                    $d[]= get_object_vars($item);
                } 
                $deallo_merge= array_merge($d, $deallocation);
                foreach($deallo_merge as $item) {
                    $this->Airtel_Anomalie_Model->insert_or_update_deallocation($item);
                }
            } else if (!empty($_SESSION["deallo"]) && !empty($_SESSION["anomalie_vi"]) && !empty($igorVI) && !empty($deallocation)) {
                $d= array();
                $v= array();

                foreach($_SESSION["deallo"] as $item) {
                    $d[]= get_object_vars($item);
                }
                foreach($_SESSION["anomalie_vi"] as $item) {
                    $v[]= get_object_vars($item);
                }

                $deallo_merge = array_merge($d, $deallocation);
                $vi_merge= array_merge($v, $igorVI);

                $this->RegulViDeallocation($vi_merge, $deallo_merge);
            }

// ------------------------------------------------------------- INSERTION ------------------------------------------------------------------------------------

            foreach($airtelIgorNormaleCO as $item) {
                $this->Igor_Airtel_Model->insert_or_update_co($item);
            }

            foreach($airtelIgorNormaleCI as $item) {
                $this->Igor_Airtel_Model->insert_or_update_ci($item);
            }

            foreach($anomalieIgorCO as $item) {
                $item["etat"]= "Non";
                $this->Igor_Airtel_Anomalie_Model->insert_or_update_co($item);
            }

            foreach($anomalieIgorCI as $item) {
                $item["etat"]= "Non";
                $this->Igor_Airtel_Anomalie_Model->insert_or_update_ci($item);
            }

            foreach ($anomalieAirtelCI as $item ) {
                $item["etat"] = "Non";
                $this->Airtel_Anomalie_Model->insert_or_update_ci($item);
            }

            foreach($anomalieAirtelCO as $item) {
                $item["etat"]= "Non";
                $this->Airtel_Anomalie_Model->insert_or_update_co($item);
            }

            foreach($ambiguous as $item) {
                $this->Airtel_Anomalie_Model->insert_or_update_ambiguous($item);
            }

            foreach($rollback as $item) {
                $this->Airtel_Anomalie_Model->insert_or_update_rb($item);
            }
            $historique = array_merge($airtelIgorNormaleCI, $airtelIgorNormaleCO, $anomalieIgorCI, $anomalieIgorCO, $anomalieAirtelCI, $anomalieAirtelCO, $igorVI, $dernierAmbiguous, $dernierRollBack, $nonAu, $deallocation);
            
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

                $this->Historique_Airtel_Model->insert_or_update($item);
            }

            foreach($nonAu as $item) {
                $this->Igor_Airtel_Model->insert_or_update_nonAu($item);
            }

            $this->exporter($airtelIgorNormaleCI, $airtelIgorNormaleCO, $dernierAmbiguous, $dernierRollBack, $anomalieAirtelCI, $anomalieAirtelCO, $anomalieIgorCI, $anomalieIgorCO, $igorVI, $deallocation, $resultat[2], $ambiguRegul);

            // redirect("importer_airtel");
            
}


// --------------------------------------------------------------------------------- INSERTION -------------------------------------------------------------

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
                if ($key !== 'LIBELLE') {
                    $resultat[$key] = str_replace(" ", "", $valeur);
                } else {
                    $resultat[$key] = $valeur;
                }
                // $resultat[$key] = str_replace(" ", "", $valeur);
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
            $resultat= array();

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
                    $item["solde"] = intval($item["amount"]) * 1;
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
            $res= array();

            foreach($data as $item) {
                if($item["description"] === "TransactionAmbiguous" || ($item["description"] === "TransactionSuccess" && empty($item["external_id"]))  && ($item["service_name"] === "BanktoWalletTransfer" || $item["service_name"] === "ChannelBanktoWalletTransfer" || $item["service_name"] === "AutoSweepMoneyTransfer" || $item["service_name"] === "WalletToBankTransfer" || $item["service_name"] === "ChannelWalletToBankTransfer") ) {
                    $resultat[] = $item;
                }
            }

            foreach($resultat as &$item) {
                if($item["service_name"] === "BanktoWalletTransfer" || $item["service_name"] === "ChannelBanktoWalletTransfer") {
                    $item["solde"] = $item["amount"] * (-1);
                    $res[] = $item;
                    
                } else if($item["service_name"] === "ChannelWalletToBankTransfer" || $item["service_name"] === "WalletToBankTransfer" || $item["service_name"] ===AutoSweepMoneyTransfer) {
                    $item["solde"] = $item["amount"] * (1);
                    $res[] = $item;
                }
            }

            return $res;
        }

        public function comparerRollBackEtAmbigous($rollback, $ambiguous) {
            $dernierRollBack = array();
            $dernierAmbiguous = array();
        
            $rollbackHash = array();
            foreach ($rollback as $itemRoll) {
                $rollbackHash[$itemRoll["reference_number"]] = true;
            }
        
            $ambiguousHash = array();
            foreach ($ambiguous as $itemAmbi) {
                $ambiguousHash[$itemAmbi["TRANSFER_ID"]] = true;
            }
        
            foreach ($rollback as $itemRoll) {
                if (!isset($ambiguousHash[$itemRoll["reference_number"]])) {
                    $dernierRollBack[] = $itemRoll;
                }
            }
        
            foreach ($ambiguous as $itemAmbi) {
                if (!isset($rollbackHash[$itemAmbi["TRANSFER_ID"]])) {
                    $dernierAmbiguous[] = $itemAmbi;
                }
            }
        
            return [$dernierRollBack, $dernierAmbiguous];
        }
        
        
        public function mergedAmbigousEtRollback($rollback, $ambiguous) {
            $resAmbi= array();
            $resRoll= array();
            $mergedRollEtAmbi= array();
            foreach($rollback as $itemRoll) {
                foreach($ambiguous as $itemAmbi) {
                    if($itemRoll["reference_number"] ===$itemAmbi["TRANSFER_ID"]) {
                        $resAmbi[]= $itemAmbi;
                        $resRoll[]= $itemRoll;
                    }
                }
            }

            $mergedRollEtAmbi[]= array_merge($resAmbi, $resRoll);
            return $mergedRollEtAmbi;
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

            $anomalieAirtel = array_map(function ($item) {
                $item['etat'] = 'Non'; 
                return $item;
            }, $anomalieAirtel);
        
            $anomalieIgor = array_map(function ($item) {
                $item['etat'] = 'Non';
                return $item;
            }, $anomalieIgor);
        
            return [$anomalieAirtel, $anomalieIgor];
        }

        public function filtrerIgorCIEtnonAu($data) {
            $igorCI= array();
            $igorVI= array();
            $nonAu = array();

            foreach($data as $item) {
                if($item["OPER"]==="CASHI" && $item["EXPL"] ==="AU") {
                    $igorCI[] = $item;
                } else if ($item["EXPL"] !=="AU" && $item["OPER"] ==="VI") {
                    $igorVI[] = $item;
                } else if ($item["EXPL"] !== "AU" && $item["OPER"] !=="VI") {
                    $data = array_change_key_case($item, CASE_LOWER);
                    $nonAu[]= $data;
                }
            }
            return [$igorCI, $igorVI, $nonAu];
        }

        public function viWithReference($data) {
            foreach($data as &$item) {
                $pattern = '/(\d{2})(\d{2})(\d{2})/';

                if (preg_match($pattern, $item["LIBELLE"], $matches)) {
                    $date = $matches[1] . '/' . $matches[2] . '/' . $matches[3];
                    $item["reference_number"]= "MBANKING" .$date ;
                } else {
                    $item["reference_number"] = NULL;
                }
            }
            return $data;
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

            $mergeDeallocationVI= array();
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
                foreach($viCopy as $j=>&$itemVi) {
                    if(intval($itemVi["MONTANT"]) == $total_amount) {
                        unset($viCopy[$j]);
                        for ($i=0; $i < count($deallocationCopy); $i++) {
                            if($deallocationCopy[$i]["reference_number"] === $reference_number) {
                                $deallocationCopy[$i]["etat"] = "Oui";
                                $deallocationCopy[$i]["REF_IGOR"]= $itemVi["REF_IGOR"]; 
                            }
                        }
                        $vi[$j]["etat"]= "Oui";
                        break;
                    }
                }
            }

            foreach ($deallocationCopy as $item) {
                $this->Airtel_Anomalie_Model->insert_or_update_deallocation($item);
            }
            foreach($vi as $item) {
                $this->Igor_Airtel_Anomalie_Model->insert_or_update_vi($item);
            }
        // return [$vi, $deallocationCopy];
    }


    public function DealloRegulVi($vi, $deallocation) {



        $viCopy= $vi;
        $deallocationCopy= $deallocation;
        $reference_number_to_total_amount = array();

        $mergeDeallocationVI= array();
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
            foreach($vi as &$itemVi) {
                if(intval($itemVi["MONTANT"]) == $total_amount) {
                    for ($i=0; $i < count($deallocationCopy); $i++) {
                        if($deallocationCopy[$i]["reference_number"] === $reference_number) {
                            $deallocationCopy[$i]["etat"] = "Oui";
                            $itemVi["reference_number"]= $reference_number;
                        }
                    }
                    $itemVi["etat"]= "Oui";
                }
            }
        }

    return [$vi, $deallocationCopy];
}


    public function RegulnonAu($nonAu, $anomalieIgor) {

        $regul= array();
        foreach ($anomalieIgor as $sessionItem) {
            foreach ($$nonAu as $igorItem) {
                if ($sessionItem->REF_IGOR === $igorItem["ref_igor"]) {
                    $sessionItem = get_object_vars($sessionItem);
                    $sessionItem["etat"] = "oui";
                    $sessionItem["date_regul"] = $igorItem["date_oper"];
                    $regul[]= $sessionItem;
                    $regul[]= $igorItem;

                    $this->Regul_Igor_Airtel_Model->update_anomalie_ci($sessionItem);
                    $this->Non_Au_Model->insert_or_update($igorItem);
                }
            }
        }
        return $regul;
    }

          
    public function changerCleRollBack($data) {
        $resultat= array();
        foreach ($data as &$item) {
            $item = array_combine(
                array_map(function ($key) {
                    return 'rollback_' . $key;
                }, array_keys($item)),
                $item
            );

            $resultat[]= $item;
        }

        return $resultat;
    }

    public function exporter($airtelIgorNormaleCI, $airtelIgorNormaleCO, $dernierAmbiguous, $dernierRollBack, $anomalieAirtelCI, $anomalieAirtelCO, $anomalieIgorCI, $anomalieIgorCO, $igorVI, $deallocation, $nonAu, $ambiguRegul) {

            $dateAujourdhui = date("Y-m-d");
            $nomFichier = "RapproAirtel-" .$dateAujourdhui .".xlsx";

            header("Content-Type: application/vnd.openxmlformats-officedocument.spreadsheetml.sheet");
            header("Content-Disposition: attachment; filename=\"" . $nomFichier . "\"");
            
            $spreadsheet = new PhpOffice\PhpSpreadsheet\Spreadsheet();
            $sheet = $spreadsheet->getActiveSheet();
            $cell_array = ["A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W"];
            $header = ["DATE OPE", "DATE VAL", "DEVISE", "MONTANT", "LIBELLE", "CODE OPER", "EXPL", "REF", "SOLDE", " ", "TRANSFER_ID", "DATE OPE", "REF", "ACCOUNT_NO", "SENDER MSISDN", "DESTMSISDN", "MONTANT", "DESCRIPTION", "SERVICE_NAME", "REFERENCE_NUMBER", "MONTANT", "SOLDE", "PRINCIPALE"];

            $sheet->mergeCells('A1:I1');
            $sheet->setCellValue('A1', 'BOA');
            $sheet->getStyle('A1')->getAlignment()->setHorizontal('center');
            $sheet->getStyle('A1:I1')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->mergeCells('K1:W1');
            $sheet->setCellValue('K1', 'AIRTEL');
            $sheet->getStyle('K1')->getAlignment()->setHorizontal('center');
            $sheet->getStyle('K1:W1')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->mergeCells('A2:I2');
            $sheet->mergeCells('K2:W2');
            $sheet->setCellValue('A2', 'PRINCIPALE');
            $sheet->getStyle('A2')->getAlignment()->setHorizontal('center');
            $sheet->getStyle('A2:I2')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->getStyle('K2:W2')->getBorders()->getAllBorders()->setBorderStyle('thin');
            $sheet->getStyle('A1')->getFont()->setBold(true);
            $sheet->getStyle('K1')->getFont()->setBold(true);
            $sheet->getStyle('A2')->getFont()->setBold(true);
            $sheet->getStyle('E3')->getFont()->setBold(true);

            
            for($i=0; $i < count($cell_array) -3 ; $i++) {
                $sheet->getStyle($cell_array[$i] . "3")->getBorders()->getAllBorders()->setBorderStyle('thin');
            }

            for($i=0; $i < count($cell_array) ; $i++) {
                $sheet->getStyle($cell_array[$i] . "4")->getBorders()->getAllBorders()->setBorderStyle('thin');
                $sheet->setCellValue($cell_array[$i] ."4", $header[$i]);
                $sheet->getStyle($cell_array[$i] . "4")->getAlignment()->setHorizontal('center');
                $sheet->getStyle($cell_array[$i] . "4")->getFont()->setBold(true);
                $sheet->getColumnDimension($cell_array[$i])->setAutoSize(true);
            }

            for($i=0; $i < count($cell_array) ; $i++) {
                $sheet->getStyle($cell_array[$i] . "4")->getBorders()->getAllBorders()->setBorderStyle('thin');
                $sheet->setCellValue($cell_array[$i] ."4", $header[$i]);
                $sheet->getStyle($cell_array[$i] . "4")->getAlignment()->setHorizontal('center');
                $sheet->getStyle($cell_array[$i] . "4")->getFont()->setBold(true);
                $sheet->getColumnDimension($cell_array[$i])->setAutoSize(true);
            }

            $lastRow = $sheet->getHighestRow() + 2;
            foreach ($airtelIgorNormaleCI as $dataRow) {
                $i=0;
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["DATE_OPER"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["DATE_VAL"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["DEVISE"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["MONTANT"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["LIBELLE"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["OPER"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["EXPL"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["REF_IGOR"]);
                $i+=2;
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["TRANSFER_ID"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["transfer_date"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["external_id"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["account_no"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["sender_msisdn"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["dest_msisdn"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["amount"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["description"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["service_name"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["reference_number"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["solde"]);
                $lastRow++;
            }

            foreach ($airtelIgorNormaleCO as $dataRow) {
                $i=0;
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["DATE_OPER"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["DATE_VAL"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["DEVISE"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["MONTANT"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["LIBELLE"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["OPER"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["EXPL"]);
                $sheet->setCellValue($cell_array[$i++].$lastRow, $dataRow["REF_IGOR"]);
                $sheet->setCellValue($cell_array[$i+2] .$lastRow, $dataRow["TRANSFER_ID"]);
                $sheet->setCellValue($cell_array[$i+3] .$lastRow, $dataRow["transfer_date"]);
                $sheet->setCellValue($cell_array[$i+4] .$lastRow, $dataRow["external_id"]);
                $sheet->setCellValue($cell_array[$i+5] .$lastRow, $dataRow["account_no"]);
                $sheet->setCellValue($cell_array[$i+6] .$lastRow, $dataRow["sender_msisdn"]);
                $sheet->setCellValue($cell_array[$i+7] .$lastRow, $dataRow["dest_msisdn"]);
                $sheet->setCellValue($cell_array[$i+8] .$lastRow, $dataRow["amount"]);
                $sheet->setCellValue($cell_array[$i+9] .$lastRow, $dataRow["description"]);
                $sheet->setCellValue($cell_array[$i+10] .$lastRow, $dataRow["service_name"]);
                $sheet->setCellValue($cell_array[$i+11] .$lastRow, $dataRow["reference_number"]);
                $sheet->setCellValue($cell_array[$i+12] .$lastRow, $dataRow["solde"]);
                
                $lastRow++;
            }

            $lastRow = $sheet->getHighestRow() + 2;
            foreach($dernierAmbiguous as $dataRow) {
                $i=10;
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["TRANSFER_ID"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["transfer_date"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["external_id"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["account_no"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["sender_msisdn"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["dest_msisdn"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["amount"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["description"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["service_name"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["reference_number"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["solde"]);
                
                $i=10;
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');

                $lastRow ++;
            }


            $lastRow = $sheet->getHighestRow() + 2;
            foreach($dernierRollBack as $dataRow) {
                $i=10;
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["TRANSFER_ID"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["transfer_date"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["external_id"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["account_no"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["sender_msisdn"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["dest_msisdn"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["amount"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["description"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["service_name"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["reference_number"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["solde"]);
                
                $i=10;
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');

                $lastRow ++;
            }
            

            $lastRow = $sheet->getHighestRow() + 2;
            foreach($deallocation as $dataRow) {
                $i=10;
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["TRANSFER_ID"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["transfer_date"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["external_id"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["account_no"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["sender_msisdn"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["dest_msisdn"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["amount"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["description"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["service_name"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["reference_number"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["solde"]);
                
                $i=10;
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('DDDDDD');

                $lastRow ++;
            }

            $lastRow = $sheet->getHighestRow() + 2;
            foreach($igorVI as $dataRow) {
                $i=0;
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["DATE_OPER"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["DATE_VAL"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["DEVISE"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["MONTANT"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["LIBELLE"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["OPER"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["EXPL"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["REF_IGOR"]);
                $i=0;
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('0000FF');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('0000FF');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('0000FF');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('0000FF');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('0000FF');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('0000FF');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('0000FF');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('0000FF');

                $lastRow ++;

            }

            $lastRow = $sheet->getHighestRow() + 2;
            foreach($anomalieIgorCI as $dataRow) {
                $i=0;
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["DATE_OPER"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["DATE_VAL"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["DEVISE"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["MONTANT"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["LIBELLE"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["OPER"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["EXPL"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["REF_IGOR"]);
                $i=0;

                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFA500');
                $lastRow ++;
            }

            $lastRow = $sheet->getHighestRow() + 2;
            foreach($nonAu as $dataRow) {
                $i=0;
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["date_oper"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["date_val"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["devise"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["montant"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["libelle"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["oper"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["expl"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["ref_igor"]);
                $i=0;

                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('90EE90');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('90EE90');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('90EE90');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('90EE90');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('90EE90');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('90EE90');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('90EE90');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('90EE90');
                $lastRow ++;
            }

            $lastRow = $sheet->getHighestRow() + 2;
            foreach($anomalieIgorCO as $dataRow) {
                $i=0;
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["DATE_OPER"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["DATE_VAL"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["DEVISE"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["MONTANT"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["LIBELLE"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["OPER"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["EXPL"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["REF_IGOR"]);
                $i=0;

                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');

                $lastRow ++;

            }

            $lastRow = $sheet->getHighestRow() + 2;

            foreach($anomalieAirtelCI as $dataRow) {
                $i=10;
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["TRANSFER_ID"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["transfer_date"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["external_id"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["account_no"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["sender_msisdn"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["dest_msisdn"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["amount"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["description"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["service_name"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["reference_number"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["solde"]);
                $i=10;
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
            }

            $lastRow = $sheet->getHighestRow() + 2;

            foreach($anomalieAirtelCO as $dataRow) {

                $i=10;
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["TRANSFER_ID"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["transfer_date"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["external_id"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["account_no"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["sender_msisdn"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["dest_msisdn"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["amount"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["description"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["service_name"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["reference_number"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["solde"]);

                $i=10;
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FF0000');
            }

            $lastRow=  $sheet->getHighestRow() + 2;

            $lastRow = $sheet->getHighestRow() + 1;
            $range = 'I5:I' . $lastRow;
            $style = $sheet->getStyle($range);

            $style->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID);
            $style->getFill()->getStartColor()->setARGB('FFFF00');

            $range = 'J5:J' . $lastRow;
            $style= $sheet->getStyle($range);
            $style->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID);
            $style->getFill()->getStartColor()->setARGB('FF00FF00');

            $range = 'V5:V' . $lastRow;
            $style= $sheet->getStyle($range);
            $style->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID);
            $style->getFill()->getStartColor()->setARGB('FF00FFFF');

            for ($row = 6; $row <= $lastRow; $row++) {
                $formula = "=D$row + I" . ($row - 1);
                $sheet->setCellValue("I$row", $formula);
            }

            for ($row = 6; $row <= $lastRow; $row++) {
                $formula = "=U$row + V" . ($row - 1);
                $sheet->setCellValue("V$row", $formula);
            }

            for ($row = 6; $row <= $lastRow; $row++) {
                $formula = "=D$row + V" . $row;
                $sheet->setCellValue("W$row", $formula);
            }

            $cell = "W1";
            $style = $sheet->getStyle($cell);
            $style->getFont()->getColor()->setARGB('FFFF0000');
            $lastRow = $sheet->getHighestRow();
            $formula = "=SUM(W6:W$lastRow)";
            $sheet->setCellValue("W1", $formula);

            $writer = new PhpOffice\PhpSpreadsheet\Writer\Xlsx($spreadsheet);
            $writer->save('php://output');
    }    
}

?>