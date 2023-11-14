<div class="tab-content">
    <div id="tabDat" class="container-fluid mt-1 tab-pane "><br>  
        <div class="row">
            <div class="col-sm-6 pl-4">
                <label for="dateDebut">Date Debut</label>
                <input type="text" class="form-control" id="min" name="min">
            </div>
            <div class="col-sm-6 pr-4">
                <label for="dateDebut">Date Fin</label>
                <input type="text" class="form-control" id="max" name="max">
            </div> 
        </div> 
        <div class="container-fluid mt-5">
            <div class="card">
                <div class="card-header">
                    <b>Liste Debit à Tort regularisé</b>
                </div>
                <div class="card-body">
                <table class="table table-bordered" id="tableDat">
                <thead style="text-align: center ;">
                    <tr>
                        <th>Compte</th>
                        <th>Date_Oper</th>
                        <th>Date_Val </th>
                        <th>Montant</th>
                        <th>Devise</th>
                        <th>Libelle</th>
                        <th>Operation</th>
                        <th>Expl</th>
                        <th>ReferenceIgor</th>
                        <th>Action</th>
                    </tr>
                </thead>
                <tbody>
                    <!-- <tr>
                        <td></td>
                        <td></td>
                        <td></td>
                        <td></td>
                        <td></td>
                        <td></td>
                        <td></td>
                        <td></td>
                        <td></td>
                        <td style="text-align: center;"><a href="#" data-toggle="modal" data-target="#myModalDat"><i class="bx bx-show" style="font-size: 20px"></i></a></td>
                    </tr> -->
                   
        <?php if (!empty($ci)) { ?>
          <?php foreach($ci as $item)  { ?>
            <tr> 
              <td><?php echo $item->COMPTE ?></td>
              <td><?php echo $item->DATE_OPER ?></td>
              <td><?php echo $item->DATE_VAL ?></td>
              <td><?php echo $item->DEVISE ?></td>
              <td><?php echo $item->MONTANT ?></td>
              <td><?php echo $item->LIBELLE ?></td>
              <td><?php echo $item->OPER ?></td>
              <td><?php echo $item->EXPL ?></td>
              <td><?php echo $item->REF_IGOR ?></td>
              <td style="text-align: center;"><a href="#" data-toggle="modal" data-target="#myModalDat<?php echo $item->id_nonau; ?>"><i class="bx bx-show" style="font-size: 20px"></i></a></td>
            </tr>
          <?php } ?>
        <?php } ?>
                </tbody>
            </table>
                </div>
            </div>
           
        </div>   
    </div>
<?php $this->load->view("pages/regul/telma/modal/modalDat"); ?>



