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
            $this->load->model("regul_igor_airtel_model");
            $this->load->model("non_au_model");

            session_start();
            
        }

        public function index() {
            $this->load->view("templates/sidebar");
            $this->load->view("pages/importer/importer");
            $this->load->view("pages/operateur/airtel-form");

            $_SESSION["last_solde"] = $this->historique_airtel_model->get_last_solde();

            $_SESSION["igor_anomalie_ci"] = $this->igor_airtel_anomalie_model->get_anomalie_ci();
            $_SESSION["igor_anomalie_co"] = $this->igor_airtel_anomalie_model->get_anomalie_co();
            $_SESSION["anomalie_vi"]= $this->igor_airtel_anomalie_model->get_vi();
            $_SESSION["deallocation"] = $this->airtel_anomalie_model->get_deallocation();
            $_SESSION["ambigu"]= $this->airtel_anomalie_model->get_ambiguous();
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
                
                for($i=1; $i < count($igor_read); $i++) {

                    $date_val= $igor_read[$i][2];
                    $date_val = date_create_from_format('d.m.Y', $date_val);
                    
                    $date_oper= $igor_read[$i][1];
                    $date_oper = date_create_from_format('d.m.Y', $date_oper);
                    
                    $igor[]= array(
                       "COMPTE" => $igor_read[$i][0],
                       "DATE_OPER" =>  date_format($date_oper, 'Y-m-d'),
                       "DATE_VAL" => date_format($date_val, 'Y-m-d'),
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
            
            // echo "--------------------------------------------------";
            

            // if(!empty($_SESSION["igor_anomalie_ci"]) && !empty($igorRegul)) {

            //     $this->RegulNonAu($igorRegul, $_SESSION["igor_anomalie_ci"]);
            //     // echo "<pre>";
            //     //     print_r($this->RegulNonAu($igorRegul, $_SESSION["igor_anomalie_ci"]));
            //     // echo "</pre>";
            // }

            // if(!empty($_SESSION["ambigu"]) && !empty($dernierRollBack)) {
                
            // }

            // if(!empty($_SESSION["deallo"]) && !empty($igorVI)) {
            //     $this->RegulViDeallocation($igorVI, $_SESSION["deallo"]);
            // } else if (!empty($_SESSION["anomalie_vi"] && !empty($deallocation))) {
            //     $this->RegulViDeallocation($_SESSION["anomalie_vi"], $deallocation);
            // } else if(!empty($deallocation) && !empty($igorVI)) {
            //     $this->RegulViDeallocation($igorVI, $deallocation);
            // }

            // echo "<pre>";
            //     print_r($airtelIgorNormaleCO);
            // echo "</pre>";




            // $this->igor_airtel_model->insert_or_update_co($airtelIgorNormaleCO);

            // echo "<pre>";
            //     print_r($resultat[]);
            // echo "</pre>";

            

            // $this->RegulViDeallocation($_SESSION["anomalie_vi"], $_SESSION["deallocation"]);
            

            // $this->RegulViDeallocation($vi, $deallo);
       
            // $this->RegulNonAu($nonAU, $_SESSION["igor_anomalie_ci"]);

// ------------------------------------------------------------- INSERTION ------------------------------------------------------------------------------------

            // foreach($airtelIgorNormaleCO as $item) {
            //     $this->igor_airtel_model->insert_or_update_co($item);
            //     // $solde_airtel+=$item["solde"];
            //     // $solde_boa+= $item["MONTANT"];
            //     // $item["solde_airtel"]=  $solde_airtel;
            //     // $item["solde_boa"]= $solde_boa;
            //     // $this->historique_airtel_model->insert($item);
            // }


            // foreach($airtelIgorNormaleCI as $item) {
            //     $this->igor_airtel_model->insert_or_update_ci($item);
            //     // $solde_airtel+=$item["solde"];
            //     // $solde_boa+= $item["MONTANT"];
            //     // $item["solde_airtel"]=  $solde_airtel;
            //     // $item["solde_boa"]= $solde_boa;
            //     // $this->historique_airtel_model->insert($item);
            // }

            // foreach($anomalieIgorCO as $item) {
            //     $this->igor_airtel_anomalie_model->insert_or_update_co($item);
            // }

            // foreach($anomalieIgorCI as $item) {
            //     $this->igor_airtel_anomalie_model->insert_or_update_ci($item);

            // }

            // foreach ($anomalieAirtelCI as $item ) {
            //     $this->airtel_anomalie_model->insert_or_update_ci($item);
            //     // $solde_airtel+=$item["solde"];
            //     // $solde_boa+= $item["MONTANT"];
            //     // $item["solde_airtel"]=  $solde_airtel;
            //     // $item["solde_boa"]= $solde_boa;
            //     // $this->historique_airtel_model->insert($item);
            // }

            // foreach($anomalieAirtelCO as $item) {
            //     $this->airtel_anomalie_model->insert_or_update_co($item);
            //     // $solde_airtel+=$item["solde"];
            //     // $solde_boa+= $item["MONTANT"];
            //     // $item["solde_airtel"]=  $solde_airtel;
            //     // $item["solde_boa"]= $solde_boa;
            //     // $this->historique_airtel_model->insert($item);
            // }

            // foreach($deallocation as $item) {
            //     $item["etat"]= "Non";
            //     $this->airtel_anomalie_model->insert_or_update_deallocation($item);
            // }



            // foreach($ambiguous as $item) {
            //     // $item["etat"] = "Oui";
            //     $this->airtel_anomalie_model->insert_or_update_ambiguous($item);
            // }

            // // foreach($dernierAmbiguous as $item) {
            // //     $item["etat"] = "Non";
            // //     $this->airtel_anomalie_model->insert_or_update_ambiguous($item);
            // // }

            // foreach($rollback as $item) {
            //     $this->airtel_anomalie_model->insert_or_update_rb($item);
            // }

            // foreach($igorVI as $item) {
            //     $item["etat"] = "Non";
            //     $this->igor_airtel_anomalie_model->insert_or_update_vi($item);
            // }

            // foreach($airtelIgorNormaleCO as $item) {
            //     $this->igor_airtel_model->insert_or_update_co($item);
            // }

            // foreach($airtelIgorNormaleCI as $item) {
            //     $this->igor_airtel_model->insert_or_update_ci($item);
            // }

            // $historique = array_merge($airtelIgorNormaleCI, $airtelIgorNormaleCO, $igorVI, $dernierAmbiguous, $dernierRollBack);
            
            // if(count($_SESSION["last_solde"]) > 0 ) {
            //     $solde_airtel= $_SESSION["last_solde"][0]->solde_airtel;
            //     $solde_boa= $_SESSION["last_solde"][0]->solde_boa;
            // } else if(count($_SESSION["last_solde"]) === 0 ) {
            //     $solde_airtel= 0;
            //     $solde_boa= 0;
            // }

            // foreach ($historique as $item) {
            //     $solde_airtel += isset($item["solde"]) ? $item["solde"] : 0;
            //     $solde_boa += isset($item["MONTANT"]) ? $item["MONTANT"] : 0;
            //     $item["solde_airtel"] = $solde_airtel;
            //     $item["solde_boa"] = $solde_boa;
            //     $this->historique_airtel_model->insert($item);
            // }

            // // echo "<pre>";
            // //     print_r($rollback);
            // // echo "</pre>";


            // $this->exporter($airtelIgorNormaleCI, $airtelIgorNormaleCO, $dernierAmbiguous, $dernierRollBack, $anomalieAirtelCI, $anomalieAirtelCO, $anomalieIgorCI, $anomalieIgorCO, $igorVI, $deallocation, $resultat[2]);

            // session_destroy();

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
                    $item["compte"] =  $item["COMPTE"];
                    $item["oper"] =  $item["OPER"];
                    $item["date_oper"] =  $item["DATE_OPER"];
                    $item["date_val"] =  $item["DATE_VAL"];
                    $item["libelle"] =  $item["LIBELLE"];
                    $item["montant"] =  $item["MONTANT"];
                    $item["ref_igor"] =  $item["REF_IGOR"];
                    $item["devise"] =  $item["DEVISE"];
                    $item["expl"] =  $item["EXPL"];

                    $cles_a_supprimer = array('COMPTE', 'OPER', "DATE_OPER", "LIBELLE", "MONTANT", "REF_IGOR", "DEVISE", "EXPL");

                    foreach ($cles_a_supprimer as $cle) {
                        if (isset($monTableau[$cle])) {
                            unset($monTableau[$cle]);
                        }
                    }

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
            // session_destroy();
        }
        
        public function RegulNonAu($nonAu, $anomalieIgor) {

            $regul= array();
            foreach ($anomalieIgor as $sessionItem) {
                foreach ($nonAu as $igorItem) {
                    if ($sessionItem->REF_IGOR === $igorItem["REF_IGOR"]) {
                        $sessionItem = get_object_vars($sessionItem);
                        $sessionItem["etat"] = "oui";
                        $sessionItem["date_regul"] = $igorItem["DATE_OPER"];
                        $regul[]= $sessionItem;
                        $regul[]= $igorItem;

                        $this->regul_igor_airtel_model->update_anomalie_ci($sessionItem);
                        $this->non_au_model->insert_or_update($igorItem);
                    }
                }
            }
            return $regul;
        }

        // public function regulRollBackAmbiguous($rollback, $ambiguous) {

        //     $regulRoll= array();
        //     $regulAmbi= array();
        //     foreach($rollback as $itemRoll) {
        //         foreach($ambiguous as $itemAmbi) {
        //             if($itemAmbi["TRANSFER_ID"]===$itemRoll["reference_number"]) {
        //                 $itemAmbi["reference_number"]= $itemRoll["reference_number"];
        //                 $item["date_regul"]= $itemRoll["date_oper"];
        //                 $this->airtel_anomalie_model->update_ambigu($itemAmbi);
        //                 $this->airtel_anomalie_model->insert_or_update_rb($itemRoll);
        //             }
        //         }
        //     }
        // }



        public function exporter($airtelIgorNormaleCI, $airtelIgorNormaleCO, $dernierAmbiguous, $dernierRollBack, $anomalieAirtelCI, $anomalieAirtelCO, $anomalieIgorCI, $anomalieIgorCO, $igorVI, $deallocation, $nonAu) {

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

            $lastRow = $sheet->getHighestRow() + 3;
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
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["DATE_OPER"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["DATE_VAL"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["DEVISE"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["MONTANT"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["LIBELLE"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["OPER"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["EXPL"]);
                $sheet->setCellValue($cell_array[$i++] .$lastRow, $dataRow["REF_IGOR"]);
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

            $lastRow = $sheet->getHighestRow() + 1;
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

                $lastRow ++;

            }

            $lastRow = $sheet->getHighestRow() + 3;
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

                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFFF00');
                $sheet->getStyle($cell_array[$i++] . $lastRow)->getFill()->setFillType(\PhpOffice\PhpSpreadsheet\Style\Fill::FILL_SOLID)->getStartColor()->setRGB('FFFF00');

                $lastRow ++;
            }


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


            // $cell = "X1";
            // $style = $sheet->getStyle($cell);
            // $style->getFont()->getColor()->setARGB('FFFF0000');
            // $lastRow = $sheet->getHighestRow();
            // $formula = "=SUM(X3:X$lastRow)";
            // $sheet->setCellValue("X1", $formula);


            // foreach ($dataRow as $key => $value) {
            //         $sheet->setCellValue($cell_array[$colIndex] . $lastRow, $value);
            //         $colIndex++;
            // }

            // $writer = new PhpOffice\PhpSpreadsheet\Writer\Xlsx($spreadsheet);
            // $writer->save('php://output');

            $writer = new PhpOffice\PhpSpreadsheet\Writer\Xlsx($spreadsheet);
            $writer->save('php://output');

    }    
}



?>